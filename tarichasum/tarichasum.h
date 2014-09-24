#include <stdio.h>

int tarichasum_main(int argc, char **argv,
	unsigned int (*hash_stream)(FILE *stream, uint8_t *out,
		unsigned int out_length),
	const char *hash_name, unsigned int hash_len, const char *program_name);
