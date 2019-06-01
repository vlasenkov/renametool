#ifndef RENAME_BSWAP
#define RENAME_BSWAP

#include <cstdint>

#ifdef _WIN32
#define byteswap_u32(x) (_byteswap_ulong(x))
#define byteswap_u16(x) (_byteswap_ushort(x))
#else
#define byteswap_u32(x) ((std::uint32_t)__builtin_bswap32(x))
#endif

#define cbyteswap_u16(x, b) ((b)?byteswap_u16(x):(x))
#define cbyteswap_u32(x, b) ((b)?byteswap_u32(x):(x))

#endif
