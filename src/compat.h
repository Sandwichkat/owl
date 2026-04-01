#ifndef AWDL_COMPAT_H_
#define AWDL_COMPAT_H_

#include <stdint.h>

#if defined(_MSC_VER) && !defined(__clang__)
#define AWDL_PACKED
#define AWDL_PACK_PUSH __pragma(pack(push, 1))
#define AWDL_PACK_POP __pragma(pack(pop))
#define AWDL_UNUSED
#else
#define AWDL_PACKED __attribute__((__packed__))
#define AWDL_PACK_PUSH
#define AWDL_PACK_POP
#define AWDL_UNUSED __attribute__((unused))
#endif

#if defined(_WIN32)

#ifndef ETHER_ADDR_LEN
#define ETHER_ADDR_LEN 6
#endif

#ifndef ETHER_MAX_LEN
#define ETHER_MAX_LEN 1518
#endif

struct ether_addr {
  uint8_t ether_addr_octet[ETHER_ADDR_LEN];
};

const char *ether_ntoa(const struct ether_addr *addr);

static inline uint16_t awdl_bswap16(uint16_t x) {
  return (uint16_t) ((x >> 8) | (x << 8));
}

static inline uint32_t awdl_bswap32(uint32_t x) {
  return ((x & 0x000000ffU) << 24) |
         ((x & 0x0000ff00U) << 8) |
         ((x & 0x00ff0000U) >> 8) |
         ((x & 0xff000000U) >> 24);
}

static inline uint64_t awdl_bswap64(uint64_t x) {
  return ((x & 0x00000000000000ffULL) << 56) |
         ((x & 0x000000000000ff00ULL) << 40) |
         ((x & 0x0000000000ff0000ULL) << 24) |
         ((x & 0x00000000ff000000ULL) << 8) |
         ((x & 0x000000ff00000000ULL) >> 8) |
         ((x & 0x0000ff0000000000ULL) >> 24) |
         ((x & 0x00ff000000000000ULL) >> 40) |
         ((x & 0xff00000000000000ULL) >> 56);
}

#ifndef htobe16
#define htobe16(x) awdl_bswap16((uint16_t) (x))
#endif
#ifndef htole16
#define htole16(x) ((uint16_t) (x))
#endif
#ifndef be16toh
#define be16toh(x) awdl_bswap16((uint16_t) (x))
#endif
#ifndef le16toh
#define le16toh(x) ((uint16_t) (x))
#endif

#ifndef htobe32
#define htobe32(x) awdl_bswap32((uint32_t) (x))
#endif
#ifndef htole32
#define htole32(x) ((uint32_t) (x))
#endif
#ifndef be32toh
#define be32toh(x) awdl_bswap32((uint32_t) (x))
#endif
#ifndef le32toh
#define le32toh(x) ((uint32_t) (x))
#endif

#ifndef htobe64
#define htobe64(x) awdl_bswap64((uint64_t) (x))
#endif
#ifndef htole64
#define htole64(x) ((uint64_t) (x))
#endif
#ifndef be64toh
#define be64toh(x) awdl_bswap64((uint64_t) (x))
#endif
#ifndef le64toh
#define le64toh(x) ((uint64_t) (x))
#endif

#else

#include <net/ethernet.h>
#if !defined(__APPLE__)
#include <netinet/ether.h>
#endif

#if defined(__APPLE__)
#include <machine/endian.h>
#include <libkern/OSByteOrder.h>

#define htobe16(x) OSSwapHostToBigInt16(x)
#define htole16(x) OSSwapHostToLittleInt16(x)
#define be16toh(x) OSSwapBigToHostInt16(x)
#define le16toh(x) OSSwapLittleToHostInt16(x)

#define htobe32(x) OSSwapHostToBigInt32(x)
#define htole32(x) OSSwapHostToLittleInt32(x)
#define be32toh(x) OSSwapBigToHostInt32(x)
#define le32toh(x) OSSwapLittleToHostInt32(x)

#define htobe64(x) OSSwapHostToBigInt64(x)
#define htole64(x) OSSwapHostToLittleInt64(x)
#define be64toh(x) OSSwapBigToHostInt64(x)
#define le64toh(x) OSSwapLittleToHostInt64(x)
#else
#include <endian.h>
#endif

#endif

#endif /* AWDL_COMPAT_H_ */
