#ifndef UTI_CREATEHASH_H
#define UTI_CREATEHASH_H

#include <_Structures/str_PairedValues.hpp>

///-------------------------------------------------------------------------------------------------------
///HASHES CORE ID WITH ABILITY ID FOR USE IN ABILITY MAP (INSTEAD OF 2D MAP)                --------------
static inline uint64_t
    Create_64b_Hash(const unsigned int h_1,
                    const unsigned int h_2)
{
    return (uint64_t)h_1 << 32 | (unsigned int)h_2;
};
///-------------------------------------------------------------------------------------------------------
///TURNS ABILITY HASH INTO SEPERATE VALUES FOR GROUP AND ABILITY                --------------------------
static inline tpPr<uint32_t>
    Unhash_64b_Hash(const uint64_t aHash)
{
    uint32_t h_1 = aHash >> 32;
    uint32_t h_2 = aHash;

    return tpPr<uint32_t>(h_1, h_2);
};

#endif // UTI_CREATEHASH_H
