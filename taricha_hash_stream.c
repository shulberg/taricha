#include <inttypes.h>
#include "taricha_hash_stream.h"
#include "taricha.h"

#define BUFFER_SIZE 8192

size_t taricha512_hash_stream(FILE *stream, uint8_t *out, size_t out_length)
{
	uint8_t buffer[BUFFER_SIZE];
	size_t bytes_read;
	struct taricha512_state s = taricha512_init();
	do
	{
		bytes_read = fread(buffer, 1, BUFFER_SIZE, stream);
		taricha512_append(buffer, bytes_read, &s);
	}
	while (!feof(stream));

	return taricha512_finalize(out, out_length, &s);
}

size_t taricha2_512_hash_stream(FILE *stream, uint8_t *out, size_t out_length)
{
	uint8_t buffer[BUFFER_SIZE];
	size_t bytes_read;
	struct taricha2_512_state s = taricha2_512_init();
	do
	{
		bytes_read = fread(buffer, 1, BUFFER_SIZE, stream);
		taricha2_512_append(buffer, bytes_read, &s);
	}
	while (!feof(stream));

	return taricha2_512_finalize(out, out_length, &s);
}

