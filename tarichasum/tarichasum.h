#include <stdio.h>

int tarichasum_main(int argc, char **argv,
	size_t (*hash_stream)(FILE *stream, uint8_t *out, size_t out_length),
	char *program_name, char *hash_name, size_t hash_len);
