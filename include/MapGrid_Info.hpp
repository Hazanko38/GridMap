#ifndef STR_MAPGRIDS_H
#define STR_MAPGRIDS_H

#include <memory>

#include <SDL2/SDL.h>

#include <_Utilities/uti_CreateHash.hpp>
#include <_Utilities/uti_FindGridLocation.hpp>

///-------------------------------------------------------------------------------------------------------
///SETUP HELPER FUNCTIONS FOR MAPGRIDINFO                     --------------------------------------------
namespace
{
    static inline size_t
        FixIfZero(size_t num)
    {
        if ( num == 0 )
            num = 1;

        return num;
    };

    static inline size_t
        CalcIfNoGridSize(const size_t totalSize,
                         const size_t sections,
                         const size_t gridSize)
     {
         if ( gridSize != 0 )
            return gridSize;

         return totalSize / sections;
     };
};

namespace GMap2D
{
    ///STRUCTURE FOR DETAILS ON ENTIRE GRIDMAP COMPOSITION
    struct GridDetails
    {
        GridDetails(){};
        explicit GridDetails(const size_t width,
                             const size_t height,
                             const size_t columns,
                             const size_t rows,
                             const size_t gSize = 0):
            w_h(width,
                height),
            c_r(FixIfZero(columns),
                FixIfZero(rows)),
            cW_rH(CalcIfNoGridSize(width, c_r.x, gSize),
                  CalcIfNoGridSize(height, c_r.y, gSize))
        {
        };

        xyPr<size_t> w_h;      ///< grid total width & height
        xyPr<size_t> c_r;      ///< grid columns and rows
        xyPr<size_t> cW_rH;    ///< grid column width & row height
    };

    ///GRID LOCATION STRUCTION
    struct GridLoc
    {
        ///Constructor for reference item
        template <typename T>
        explicit GridLoc(const GridDetails& gridInfo,
                         const T& loc):
            x_y(loc.x, loc.y),
            c_r(implem::FindGridLoc(loc.x, gridInfo.w_h.x, gridInfo.cW_rH.x),
                implem::FindGridLoc(loc.y, gridInfo.w_h.y, gridInfo.cW_rH.y)),
            gHash(Create_64b_Hash(c_r.y, c_r.x)),
            b_bad_loc(c_r.y > gridInfo.c_r.y || c_r.x > gridInfo.c_r.x)
        {
        };

        const xyPr<size_t> x_y;         ///< item x & y location
        const xyPr<size_t> c_r;         ///< item column and row
        const uint64_t gHash;           ///< item row+col location hash
        const bool b_bad_loc;           ///< verification of if GridLoc created is valid
    };
};

#endif // STR_MAPGRIDS_H
