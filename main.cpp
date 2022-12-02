#include <iostream>

#include <MapGrid.hpp>
#include <MapGrid_Info.hpp>

#include <_Utilities/uti_FindInside.hpp>
#include <_Utilities/uti_RandomGenerator.h>

static constexpr size_t TOTAL_AMOUNT = 5000;
static constexpr size_t CHECK_TIMES  = 10;

const int mapWidth  = 500000;
const int mapHeight = 500000;
const int mapCols = 12;
const int mapRows = 12;
const int mapWidth_s  = mapWidth / 1000;
const int mapHeight_s = mapHeight / 1000;

const int rectSize = 350;

uti_RandomGenerator uti_RandomGenerator::instance;

///GLOBALS
auto mapGridInfo = GMap2D::GridDetails(mapWidth,
                                       mapHeight,
                                       mapCols,
                                       mapRows);

struct testStruct
{
    xyPr<int>* GetLocation() { return &loc ;}
    void SetLocation(const xyPr<int>& l)
    {
        loc = l;
    };

    private:
        xyPr<int> loc;
};

struct Rect
{
    int x = 0;
    int y = 0;
    int w = 0;
    int h = 0;

};

GMap2D::GridMap<Rect, Rect*> gridMap;

///-------------------------------------------------------------------------------------------------------
/// GRID MAP TEST
namespace CheckGridMap
{
    ///
    void
        BuildOriginalList_GridMap()
    {
        const auto beforeTime = std::chrono::high_resolution_clock::now();

        const uint64_t totalAmount = TOTAL_AMOUNT;

        gridMap.GridMap_Init(mapGridInfo);

        for ( uint64_t i = 0; i < totalAmount; i++ )
        {
            Rect tLoc;
            tLoc.x = uti_RandomGenerator::Instance().GenerateRandom() * mapWidth_s;
            tLoc.y = uti_RandomGenerator::Instance().GenerateRandom() * mapHeight_s;
            tLoc.w = rectSize;
            tLoc.h = rectSize;

            assert(tLoc.x < mapWidth && tLoc.y < mapHeight);

            gridMap.Add(tLoc);
        }

        const auto afterTime  = std::chrono::high_resolution_clock::now();
        const auto listBuildTime = std::chrono::duration_cast<std::chrono::microseconds>
                                        (afterTime - beforeTime);

        //REPORT TIME FOR BUILDING LIST
        std::cout << "Time to build gridMap list: " << listBuildTime.count() << "ms" << std::endl;
    };
    ///
    void
        RunDistanceCheck_GridMap()
    {
        const auto beforeTime = std::chrono::high_resolution_clock::now();

        const size_t totalCheck = CHECK_TIMES;

        size_t itemsChecked = 0;
        size_t itemsChecked_total = 0;
        size_t collisions = 0;

        //Checks totalChecks(N) amount of times
        for ( size_t i = 0; i < totalCheck; i++ )
        {
            //builds Hash map to check off from
            const auto beforeCreateTime = std::chrono::high_resolution_clock::now();
            //Repopulate GridCells with objects based on location
            gridMap.Refresh();

            const auto afterCreateTime = std::chrono::high_resolution_clock::now();
            const auto gridCreateTime = std::chrono::duration_cast<std::chrono::microseconds>
                                            (afterCreateTime - beforeCreateTime);
            std::cout << "Time to Refresh GridMap: " << gridCreateTime.count() << std::endl;

            //Checks all items in list against all other items
            const auto & listItems = gridMap.List();
            for ( auto & testItem : listItems )
            {
                auto cLoc = GMap2D::GridLoc(mapGridInfo,
                                             testItem);

                const auto & localCells = gridMap.Local(cLoc);

                for ( auto & checkItem : localCells )
                {
                    if ( &testItem == checkItem ) continue;

                    bool inside = FindInside_Radius_Q(testItem,
                                                      *checkItem);
                    if ( inside )
                        collisions++;

                    itemsChecked_total++;
                }
                itemsChecked++;
            }

        }

        const auto afterTime = std::chrono::high_resolution_clock::now();
        const auto distCheckTime = std::chrono::duration_cast<std::chrono::microseconds>
                                        (afterTime - beforeTime);

        //REPORT TIME FOR RUNNING DISTANCE CHECK
        std::cout << "Time to check distance: " << distCheckTime.count() << "ms" << std::endl;
        std::cout << "Items Checked: " << itemsChecked << std::endl;
        std::cout << "Total Items Checked: " << itemsChecked_total << std::endl;
        std::cout << "Total Collisions: " << collisions << std::endl;
    };
};

int main()
{
    CheckGridMap::BuildOriginalList_GridMap();
    //CHECK MAPGRID DISTANCES - MAPGRID CUSTOM STRUCTURE ONLY
    CheckGridMap::RunDistanceCheck_GridMap();

    return 0;
}
