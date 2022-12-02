#ifndef UTI_CREATEGRIDMAP_H
#define UTI_CREATEGRIDMAP_H

#include <vector>

#include <MapGrid_Info.hpp>
#include <_Utilities/uti_FindGridLocation.hpp>

namespace GMap2D
{
    template <typename T, typename U>
    class GridMap;

    namespace implem
    {
        ///-------------------------------------------------------------------------------------------------------
        ///LOAD GRIDMAP CELLS WITH ITEMS RESIDING WITHIN EACH CELL(GRID) LOCATION               ------------------
        ///FOR ( COPY OR REFERENCE ) MAPGRID ITEM TYPE                                          ------------------
        template <typename T, typename U>
        static inline void
            LoadGridCells(GridMap<T, U>* gridMap)
        {
            const auto & gInfo = gridMap->gridInfo;
            auto & fullList = gridMap->listItems;

            for ( auto & item : fullList )
            {
                const auto row = FindGridLoc(item.y, gInfo.w_h.y, gInfo.cW_rH.y);     //Find row to insert into
                const auto col = FindGridLoc(item.x, gInfo.w_h.x, gInfo.cW_rH.x);     //Find column to insert into

                auto & itemCell = gridMap->FindCell(row, col);
                itemCell.gridItems_cell.push_back(&item);
            }
        };
        ///-------------------------------------------------------------------------------------------------------
        ///LOAD GRIDMAP CELLS WITH ITEMS RESIDING WITHIN EACH CELL(GRID) LOCATION               ------------------
        ///FOR ( RAW POINTER ) MAPGRID ITEM TYPE                                                ------------------
        template <typename T, typename U>
        static inline void
            LoadGridCells(GridMap<T*, U>* gridMap)
        {
            const auto & gInfo = gridMap->gridInfo;
            auto & fullList = gridMap->listItems;

            for ( auto & item : fullList )
            {
                const auto & itemLoc = item->GetLocation();

                const auto row = FindGridLoc(itemLoc->y, gInfo.w_h.y, gInfo.cW_rH.y);     //Find row to insert into
                const auto col = FindGridLoc(itemLoc->x, gInfo.w_h.x, gInfo.cW_rH.x);     //Find column to insert into

                auto & itemCell = gridMap->FindCell(row, col);
                itemCell.gridItems_cell.push_back(item);
            }
        };
        ///-------------------------------------------------------------------------------------------------------
        ///LOAD GRIDMAP CELLS WITH ITEMS RESIDING WITHIN EACH CELL(GRID) LOCATION               ------------------
        ///FOR ( SHARED_PTR ) MAPGRID ITEM TYPE                                                 ------------------
        template <typename T, typename U>
        static inline void
            LoadGridCells(GridMap<std::shared_ptr<T>, std::shared_ptr<U>>* gridMap)
        {
            const auto & gInfo = gridMap->gridInfo;
            auto & fullList = gridMap->listItems;

            for ( auto & item : fullList )
            {
                const auto & itemLoc = item->GetLocation();

                const auto row = FindGridLoc(itemLoc->y, gInfo.w_h.y, gInfo.cW_rH.y);     //Find row to insert into
                const auto col = FindGridLoc(itemLoc->x, gInfo.w_h.x, gInfo.cW_rH.x);     //Find column to insert into

                auto & itemCell = gridMap->FindCell(row, col);
                itemCell.gridItems_cell.push_back(item);
            }
        };
        ///-------------------------------------------------------------------------------------------------------
        ///LOAD GRIDMAP CELLS WITH ITEMS RESIDING WITHIN EACH CELL(GRID) LOCATION               ------------------
        ///FOR ( SHARED_PTR ) MAPGRID ITEM TYPE                                                 ------------------
        template <typename T, typename U>
        static inline void
            LoadGridCells(GridMap<std::shared_ptr<T>, U>* gridMap)
        {
            const auto & gInfo = gridMap->gridInfo;
            auto & fullList = gridMap->listItems;

            for ( auto & item : fullList )
            {
                const auto & itemLoc = item->GetLocation();

                const auto row = FindGridLoc(itemLoc->y, gInfo.w_h.y, gInfo.cW_rH.y);     //Find row to insert into
                const auto col = FindGridLoc(itemLoc->x, gInfo.w_h.x, gInfo.cW_rH.x);     //Find column to insert into

                auto & itemCell = gridMap->FindCell(row, col);
                itemCell.gridItems_cell.push_back(item.get());
            }
        };

        ///-------------------------------------------------------------------------------------------------------
        ///JOINS GRID MAP SECTION INTO ONE VECTOR BASED ON CHAR LOCATION IN GRID            ----------------------
        template <typename T, typename U>
        static inline std::vector<U>
            JoinGridCells(GridMap<T, U>* gridMap,
                          const GridLoc& gLoc)
        {
            std::vector<U> tVec;

            const auto & gInfo = gridMap->gridInfo;

            //Rows start-end to check
            size_t gTop   = gLoc.c_r.y;
            size_t gBot   = gLoc.c_r.y + 1;     //Pre-increment Bottom point
            //Columns start-end to check
            size_t gLeft  = gLoc.c_r.x;
            size_t gRight = gLoc.c_r.x + 1;     //Pre-increment Right point

            //Set to adjacent grid cells if not at edge
            if ( gTop > 0 )
                gTop--;
            if ( gBot > gInfo.c_r.y - 1 )
                gBot = gInfo.c_r.y - 1;
            if ( gLeft > 0 )
                gLeft--;
            if ( gRight > gInfo.c_r.x - 1 )
                gRight = gInfo.c_r.x - 1;

            size_t startR = gTop;               //Starting Row to check
            size_t startC = gLeft;              //Starting Column to check
            while ( startR <= gBot )            //Add all Rows from Top to Bottom
            {
                while ( startC <= gRight )      //Add all Columns from Left to Right
                {
                    const auto & cellItems = gridMap->FindCell(startR, startC).gridItems_cell;

                    //Insert entire contents of Row&Col vector index into temp vector
                    tVec.insert(tVec.end(),
                                cellItems.begin(),
                                cellItems.end());
                    startC++;
                }
                startC = gLeft;                 //Reset to Far Left on next Row
                startR++;
            }

            return tVec;
        };
        ///-------------------------------------------------------------------------------------------------------
        ///JOINS GRID MAP SECTION INTO ONE VECTOR BASED ON ZOOM IN GRID         ----------------------------------
        template <typename T, typename U>
        static inline std::vector<U>
            JoinGridCells(GridMap<T, U>* gridMap,
                          const SDL_Rect& gRect)
        {
            std::vector<U> tVec;

            const auto & gInfo = gridMap->gridInfo;

            //Grid location of top left of rectangle
            const auto tl_point = GridLoc(gInfo,
                                          xyPr<int>(gRect.x,
                                                    gRect.y));
            //Grid location of bot right of rectangle
            const auto br_point = GridLoc(gInfo,
                                          xyPr<int>(gRect.x + gRect.w,
                                                    gRect.y + gRect.h));

            //Rows start-end to check
            size_t gTop   = tl_point.c_r.y;
            size_t gBot   = br_point.c_r.y + 1; //Pre-increment Bottom point
            //Columns start-end to check
            size_t gLeft  = tl_point.c_r.x;
            size_t gRight = br_point.c_r.x + 1; //Pre-increment Right point

            //Set to adjacent grid cells if not at edge
            if ( gTop > 0 )
                gTop--;
            if ( gBot > gInfo.c_r.y - 1 )
                gBot = gInfo.c_r.y - 1;
            if ( gLeft > 0 )
                gLeft--;
            if ( gRight > gInfo.c_r.x - 1 )
                gRight = gInfo.c_r.x - 1;

           size_t startR = gTop;                //Starting Row to check
           size_t startC = gLeft;               //Starting Column to check
            while ( startR <= gBot )            //Add all Rows from Top to Bottom
            {
                while ( startC <= gRight )      //Add all Columns from Left to Right
                {
                    const auto & cellItems = gridMap->FindCell(startR, startC).gridItems_cell;

                    //Insert entire contents of Row&Col vector index into temp vector
                    tVec.insert(tVec.end(),
                                cellItems.begin(),
                                cellItems.end());
                    startC++;
                }
                startC = gLeft;                     //Reset to Far Left on next Row
                startR++;
            }

            return tVec;
        };
    };

};  //end of GMap2D namespace

#endif // UTI_CREATEGRIDMAP_H
