#ifndef BYTESWAP_HPP_
#define BYTESWAP_HPP_

namespace byteswap {

#include <cstdint>
#include <cstdlib>

    // 2-byte unsigned byte swap
    inline uint16_t u16(uint16_t x) {
#ifdef _WIN32
        return _byteswap_ushort(x);
#else
        return x << 8 | x >> 8;
#endif
    }

    // 4-byte unsigned byte swap
    inline uint32_t u32(uint32_t x) {
#ifdef _WIN32
        return _byteswap_ulong(x);
#else
        return __builtin_bswap32(x);
#endif
    }

    // 2-byte conditional unsigned byte swap
    inline uint16_t u16c(uint16_t x, bool swap) {
        return swap ? u16(x) : x;
    }

    // 4-byte conditional unsigned byte swap
    inline uint32_t u32c(uint32_t x, bool swap) {
        return swap ? u32(x) : x;
    }

}

#endif
