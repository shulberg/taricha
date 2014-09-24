#include <stdio.h>
#include <stdint.h>

unsigned int taricha512_hash_stream(FILE *stream, uint8_t *out,
	   unsigned int out_length);

unsigned int taricha2_512_hash_stream(FILE *stream, uint8_t *out,
		unsigned int out_length);
