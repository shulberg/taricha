#ifndef _TARICHA_H
#define _TARICHA_H

#include <stdint.h>

struct taricha512_state
{
	uint8_t buffer[64];
	size_t buffer_free;
	uint64_t s[8];
};

struct taricha512_state taricha_init();

void taricha512_append(const uint8_t *in, size_t length,
		struct taricha512_state *s);

size_t taricha512_finalize(uint8_t *out, size_t length,
		struct taricha512_state *s);

size_t taricha512_full(const uint8_t *in, uint8_t *out,
		size_t in_length, size_t out_length);

#endif
