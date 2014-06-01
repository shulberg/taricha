#ifndef _TARICHA_H
#define _TARICHA_H

#include <stdint.h>

struct taricha_state
{
	uint8_t buffer[64];
	size_t buffer_free;
	uint64_t s[8];
};

struct taricha_state taricha_init();

void taricha_append(const uint8_t *in, size_t length,
		struct taricha_state *s);

void taricha_finalize(uint8_t *out, size_t length,
		struct taricha_state *s);

void taricha_full(const uint8_t *in, uint8_t *out,
		size_t in_length, size_t out_length);

#endif
