#ifndef UTI_FINDINSIDE_H
#define UTI_FINDINSIDE_H

#include <cmath>

#include <_Structures/str_PairedValues.hpp>
#include <_Utilities/uti_FindDistance.hpp>

///-------------------------------------------------------------------------------------------------------
///FINDS IF SELF IS INSIDE OF TARGET RECTANGLE                  ------------------------------------------
template <typename T, typename U>
static inline bool
    FindInside_Square(const T& self,   ///<Expects structure with x|y members
                      const U& target) ///<Expects structure with w|h|x|y members
{
    const int dist_x = abs(target.x - static_cast<int>(self.x)) - target.w;

    //not inside X can return no collision
    if ( !signbit(dist_x) )
        return false;

    const int dist_y = abs(target.y - static_cast<int>(self.y)) - target.h;

    //not inside Y can return no collision
    if ( !signbit(dist_y) )
        return false;

    //collision
    return true;
};

///-------------------------------------------------------------------------------------------------------
///FINDS IF SELF IS INSIDE OF TARGET RADIUS                     ------------------------------------------
template <typename T, typename U>
static inline bool
    FindInside_Radius_Q(const T& self,   ///<Expects structure with x|y members
                        const U& target) ///<Expects structure with w|h|x|y members
{
    //distance of both X and Y
    const auto distXY = xyPr<int>(target.x - self.x,
                                  target.y - self.y);

    //perform quicker ( but less accurate ) distance check first
    const int qkDist = FindQuickDistance(distXY.x, distXY.y);
    const int double_size = target.w + target.w;
    //outside of quick distance can return no collision
    if ( qkDist > double_size )
        return false;

    //perform accurate distance check
    const int distance = FindDistance(distXY.x, distXY.y);
    //outside of accurate distance can return no collision
    if ( distance > target.w )
        return false;

    //collision
    return true;
};

///-------------------------------------------------------------------------------------------------------
///FINDS IF SELF IS INSIDE OF TARGET RADIUS                     ------------------------------------------
template <typename T, typename U>
static inline bool
    FindInside_Radius(const T& self,   ///<Expects structure with x|y members
                      const U& target) ///<Expects structure with w|h|x|y members
{
    //distance of both X and Y
    const auto distXY = xyPr<int>(target.x - self.x,
                                  target.y - self.y);

    //perform accurate distance check
    const int distance = FindDistance(distXY.x, distXY.y);
    //outside of accurate distance can return no collision
    if ( distance > target.w )
        return false;

    //collision
    return true;
};

///-------------------------------------------------------------------------------------------------------
///FINDS IF SELF IS INSIDE OF TARGET RECTANGLE                  ------------------------------------------
template <typename T>
static inline bool
    FindOcclude_Square(const T& self,   ///<Expects structure with w|h|x|y members
                       const T& target) ///<Expects structure with w|h|x|y members
{
    const int self_half_x   = self.w * 0.5f;
    const int target_half_x = target.w * 0.5f;
    const int total_half_x  = self_half_x + target_half_x;
    const int distance_x    = abs(target.x - self.x) - total_half_x;

    //not inside X can return no collision
    if ( !signbit(distance_x) )
        return false;

    const int self_half_y   = self.h * 0.5f;
    const int target_half_y = target.h * 0.5f;
    const int total_half_y  = self_half_y + target_half_y;
    const int distance_y    = abs(target.y - self.y) - total_half_y;

    //not inside Y can return no collision
    if ( !signbit(distance_y) )
        return false;

    //collision
    return true;
};

///-------------------------------------------------------------------------------------------------------
///FINDS IF SELF IS INSIDE OF TARGET RADIUS                     ------------------------------------------
template <typename T>
static inline bool
    FindOcclude_Radius_Q(const T& self,   ///<Expects structure with w|h|x|y members
                         const T& target) ///<Expects structure with w|h|x|y members
{
    //distance of both X and Y
    const auto distXY = xyPr<int>(target.x - self.x,
                                  target.y - self.y);

    //perform quicker ( but less accurate ) distance check first
    const int qkDist = FindQuickDistance(distXY.x, distXY.y);
    const int both_size = self.w + target.w;
    //outside of quick distance can return no collision
    if ( qkDist > both_size )
        return false;

    //perform accurate distance check
    const int distance = FindDistance(distXY.x, distXY.y);
    const int both_halves = self.w * 0.5f + target.w * 0.5f;
    //outside of accurate distance can return no collision
    if ( distance > both_halves )
        return false;

    //collision
    return true;
};

///-------------------------------------------------------------------------------------------------------
///FINDS IF SELF IS INSIDE OF TARGET RADIUS                     ------------------------------------------
template <typename T>
static inline bool
    FindOcclude_Radius(const T& self,   ///<Expects structure with w|h|x|y members
                       const T& target) ///<Expects structure with w|h|x|y members
{
    //distance of both X and Y
    const auto distXY = xyPr<int>(target.x - self.x,
                                  target.y - self.y);

    //perform accurate distance check
    const int distance = FindDistance(distXY.x, distXY.y);
    const int both_halves = self.w * 0.5f + target.w * 0.5f;
    //outside of accurate distance can return no collision
    if ( distance > both_halves )
        return false;

    //collision
    return true;
};

#endif // UTI_FINDINSIDE_H
