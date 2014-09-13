#ifndef _TARICHA_H
#define _TARICHA_H

#include <stdint.h>
#include <stdlib.h>

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT
#endif

struct taricha512_state
{
	uint8_t buffer[64];
	size_t buffer_free;
	uint64_t s[8];
};

struct taricha512_state EXPORT taricha512_init();

void EXPORT taricha512_append(const uint8_t *in, size_t length,
		struct taricha512_state *s);

size_t EXPORT taricha512_finalize(uint8_t *out, size_t length,
		struct taricha512_state *s);

size_t EXPORT taricha512_full(const uint8_t *in, uint8_t *out,
		size_t in_length, size_t out_length);

uint64_t EXPORT taricha512_64_finalize(struct taricha512_state *s);

uint64_t EXPORT taricha512_64_full(const uint8_t *in, size_t in_length);


struct taricha2_512_state
{
	uint8_t buffer[128];
	size_t buffer_free;
	uint64_t s[8];
};

struct taricha2_512_state EXPORT taricha2_512_init();

void EXPORT taricha2_512_append(const uint8_t *in, size_t length,
		struct taricha2_512_state *s);

size_t EXPORT taricha2_512_finalize(uint8_t *out, size_t length,
		struct taricha2_512_state *s);

size_t EXPORT taricha2_512_full(const uint8_t *in, uint8_t *out,
		size_t in_length, size_t out_length);

uint64_t EXPORT taricha2_512_64_finalize(struct taricha2_512_state *s);

uint64_t EXPORT taricha2_512_64_full(const uint8_t *in, size_t in_length);
#endif
