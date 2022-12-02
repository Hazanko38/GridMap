#ifndef STR_MAPGRID_H
#define STR_MAPGRID_H

#include <iostream>
#include <assert.h>

#include <MapGrid_Info.hpp>
#include <_Utilities/uti_CreateHash.hpp>
#include <_Utilities/uti_CreateGridMap.hpp>

namespace GMap2D
{
    using namespace implem;

    template <typename T, typename U>
    class GridMap
    {
        struct GridCell
        {
            void Clear()
            {
                gridItems_cell.clear();
                gridItems_local.clear();
                b_local_populated = false;
            };

            std::vector<U> gridItems_cell;   //Items current to cell
            std::vector<U> gridItems_local;  //Items local to cell
            bool b_local_populated = false;  //Have gridItems been populated yet
        };

        //Load Cells from listItems unsorted vector
        friend void
            LoadGridCells<>(GridMap<T, U>* gridMap);
        //Join Local(Neighboring) cells into single vector using GridLoc
        friend std::vector<U>
            JoinGridCells<>(GridMap<T, U>* gridMap,
                            const GridLoc& gLoc);
        //Join Local(Neighboring) cells into single vector using area rect
        friend std::vector<U>
            JoinGridCells<>(GridMap<T, U>* gridMap,
                            const SDL_Rect& gRect);

        typedef void
            (GridMap::*initFunc)();
        typedef void
            (GridMap::*closeFunc)();

        public:
            GridMap(){};
            ~GridMap()
            {
                (this->*grid_close_func)();
            };
            ///Initializes GridMap Cells based on given GridDetails
            ///--- ( THIS NEEDS TO BE CALLED BEFORE FIRST REFRESH )
            void
                GridMap_Init(const GridDetails& gInfo)
            {
                gridInfo = gInfo;
                cellTotal = gInfo.c_r.x * gInfo.c_r.y;

                (this->*grid_init_func)();
            };

            ///Clear & Recreate arrays within all cells
            ///--- ( NEED TO RUN THIS EVERY FRAME ITEM CHANGES LOCATION )
            void
                Refresh()
            {
                Clear();
                LoadGridCells(this);
            };

            ///Clear all cell information and set to new state
            ///--- ( DOES NOT NEED TO BE USED EXTERNALLY IF USING )
            ///--- ( REFRESH FUNCTION TO KEEP THINGS MAINTAINED )
            void
                Clear()
            {
                for ( size_t i = 0; i < cellTotal; i++ )
                    gridCells[i].Clear();
            };

            ///Add item to unsorted list contained within GridMap
            ///--- ( Set optional bool to true to refresh all Cells )
            void
                Add(const T& newItem,
                    bool refresh = false)
            {
                listItems.push_back(newItem);

                if ( refresh )
                    Refresh();
            };

            ///REMOVE BY LOCATION REFERENCE - REQUIRES NON-PTR ITEM WITH X|Y VARS
            ///Remove item via matching location to list item location
            ///--- ( Set optional bool to true to clear all cells )
            ///--- ( instead of just single containing cell )
            inline void
                Remove_Loc(const T& rmvLoc,
                             bool clearAll = false)
            {
                listItems.erase(std::remove_if(listItems.begin(),
                                               listItems.end(),
                                               [rmvLoc](T item) { return item.x == rmvLoc.x && item.y == rmvLoc.y; }),
                                               listItems.end());

                //Clear all cells
                if ( clearAll )
                {
                    Clear();
                    return;
                }
                //Clear only specific cell
                auto & cell = FindCell(rmvLoc);
                cell.Clear();
            };
            ///
            inline void
                Remove_Ptr(T rmvItem,
                           bool clearAll = false)
            {
                const auto itemLoc = rmvItem->GetLocation();
                listItems.erase(std::remove_if(listItems.begin(),
                                               listItems.end(),
                                               [rmvItem](T item) { return item == rmvItem; }),
                                               listItems.end());

                if ( clearAll )
                {
                    Clear();
                    return;
                }

                auto & cell = FindCell(*itemLoc);
                cell.Clear();
            };

            ///Return requested cell via row and column
            ///--- ( NO BOUND CHECKING IS PERFORMED HERE - USE failure bool set on GridLoc )
            const auto&
                operator()(const size_t row,
                           const size_t col)
            {
                const size_t cellIndex = CalcIndex(row,
                                                   col);

                return gridCells[cellIndex];
            };

            ///Return requested cell via 64bit gridHash
            ///--- ( NO BOUND CHECKING IS PERFORMED HERE - USE failure bool set on GridLoc )
            const auto&
                operator[](const uint64_t gHash)
            {
                auto hash_dc = Unhash_64b_Hash(gHash);

                const size_t cellIndex = CalcIndex(hash_dc._1,
                                                   hash_dc._2);

                return gridCells[cellIndex];
            };

            ///Returns full unsorted list contained within GridMap
            inline auto&
                List()
            {
                return listItems;
            };

            ///Returns direct reference to current cell's array of contained items
            ///-This array does not include neighbors(local area arrays)
            ///--- ( NO BOUND CHECKING IS PERFORMED HERE - USE failure bool set on GridLoc )
            inline const auto&
                Cell(const GridLoc& gLoc)
            {
                //Return single array for items residing in current cell
                return FindCell(gLoc.c_r.y,
                                gLoc.c_r.x).gridItems_cell;
            };

            ///Returns direct reference to local area array surrounding GridLoc Row+Col
            ///-A single contiguous array is maintained containing items from cells in local area
            ///-Up to 9 vectors of cells located left-right & up-down up current GridLoc
            ///--- ( this is to reduce lookup performance hit & improve pre-cache )
            ///--- ( NO BOUND CHECKING IS PERFORMED HERE - USE failure bool set on GridLoc )
            inline const auto&
                Local(const GridLoc& gLoc)
            {
                //Find the cell located where GridLoc is
                auto & currCell = FindCell(gLoc.c_r.y,
                                           gLoc.c_r.x);

                //Already populated can return local area array immediately
                if ( currCell.b_local_populated )
                    return currCell.gridItems_local;

                //Populate local area array with spliced vectors
                currCell.gridItems_local = JoinGridCells(this,
                                                         gLoc);
                currCell.b_local_populated = true;

                //Return single local area array for GridLoc current cell
                return currCell.gridItems_local;
            };
            ///
            inline const auto&
                Local(const SDL_Rect& gRect)
            {
                auto cLoc = GridLoc(gridInfo, gRect);
                assert(!cLoc.b_bad_loc);

                //Find the cell located where GridLoc is
                auto & currCell = FindCell(cLoc.c_r.y,
                                           cLoc.c_r.x);

                //Already populated can return local area array immediately
                if ( currCell.b_local_populated )
                    return currCell.gridItems_local;

                //Populate local area array with spliced vectors
                currCell.gridItems_local = JoinGridCells(this,
                                                         gRect);
                currCell.b_local_populated = true;

                //Return single local area array for GridLoc current cell
                return currCell.gridItems_local;
            };

        private:
            initFunc grid_init_func = &GridMap::Init_Reserve;
            closeFunc grid_close_func = &GridMap::Void_Func;

            GridCell* gridCells;     ///< All cells contained within GridMap
            std::vector<T> listItems;   ///< Entire unsorted list of items in GridMap

            GridDetails gridInfo;
            size_t cellTotal = 0;

            ///Calculate index start of the row
            inline size_t
                CalcIndex(const int row)
            {
                return row * gridInfo.c_r.x;
            };
            ///Calculate full cell index of the row & col
            inline size_t
                CalcIndex(const int row,
                          const int col)
            {
                return CalcIndex(row) + col;
            };

            ///Returns direct reference to requested cell
            inline GridCell&
                FindCell(const int row,
                         const int col)
            {
                //Calculate position in GridMap index using position & GridDetails
                const size_t gridIndex = CalcIndex(row,
                                                   col);

                //Return requested Cell
                return gridCells[gridIndex];
            };
            ///Returns direct reference to requested cell & builds own GridLoc
            template <typename H>
            inline GridCell&
                FindCell(const H& gLoc)
            {
                auto cLoc = GridLoc(gridInfo,
                                    gLoc);

                return FindCell(cLoc.c_r.y,
                                cLoc.c_r.x);
            };
            ///Creates GridCells and set init+close functions
            void
                Init_Reserve()
            {
                gridCells = new GridCell[cellTotal];
                grid_init_func  = &GridMap::Void_Func;
                grid_close_func = &GridMap::Close_Reserve;
            };
            ///Deletes GridCells if created and set close function
            void
                Close_Reserve()
            {
                delete[] gridCells;
                grid_close_func = &GridMap::Void_Func;
            };
            ///Function does nothing
            void
                Void_Func()
            {};
    };

};

#endif // STR_MAPGRID_H
