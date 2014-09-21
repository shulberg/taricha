#include <stdint.h>

#ifdef _WIN32
#define inline __inline
#endif

static inline uint64_t rotl64(uint64_t x, uint8_t r)
{
	return (x<<r) | (x>>(64-r));
}

static inline uint64_t correct_bytes(uint64_t x)
{
#ifdef BIG_ENDIAN
	x = (x >> 56) & 0x00000000000000FF |
		(x >> 40) & 0x000000000000FF00 |
		(x >> 24) & 0x0000000000FF0000 |
		(x >>  8) & 0x00000000FF000000 |
		(x <<  8) & 0x000000FF00000000 |
		(x << 24) & 0x0000FF0000000000 |
		(x << 40) & 0x00FF000000000000 |
		(x << 56) & 0xFF00000000000000 ;
#endif
	return x;
}

static inline void mix512x(uint64_t y[8],
		uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
		uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7,
		uint8_t  r0, uint8_t  r1, uint8_t  r2, uint8_t  r3,
		uint8_t  r4, uint8_t  r5, uint8_t  r6, uint8_t  r7)
{
	y[0] = rotl64(y[0] ^ x0, r0);
	y[1] = rotl64(y[1] ^ x1, r1);
	y[2] = rotl64(y[2] ^ x2, r2);
	y[3] = rotl64(y[3] ^ x3, r3);
	y[4] = rotl64(y[4] ^ x4, r4);
	y[5] = rotl64(y[5] ^ x5, r5);
	y[6] = rotl64(y[6] ^ x6, r6);
	y[7] = rotl64(y[7] ^ x7, r7);
}

static inline void mix512a(uint64_t y[8],
		uint64_t x0, uint64_t x1, uint64_t x2, uint64_t x3,
		uint64_t x4, uint64_t x5, uint64_t x6, uint64_t x7,
		uint8_t  r0, uint8_t  r1, uint8_t  r2, uint8_t  r3,
		uint8_t  r4, uint8_t  r5, uint8_t  r6, uint8_t  r7)
{
	y[0] = rotl64(y[0] + x0, r0);
	y[1] = rotl64(y[1] + x1, r1);
	y[2] = rotl64(y[2] + x2, r2);
	y[3] = rotl64(y[3] + x3, r3);
	y[4] = rotl64(y[4] + x4, r4);
	y[5] = rotl64(y[5] + x5, r5);
	y[6] = rotl64(y[6] + x6, r6);
	y[7] = rotl64(y[7] + x7, r7);
}
