#include <stdio.h>
#include <stdint.h>

size_t taricha512_hash_stream(FILE *stream, uint8_t *out, size_t out_length);

size_t taricha2_512_hash_stream(FILE *stream, uint8_t *out, size_t out_length);
