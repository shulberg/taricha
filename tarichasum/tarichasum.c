#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "options.h"
#include "processing.h"
#include "tarichasum.h"

const char *HELP_TEXT =
"Usage: %s [OPTIONS] [--] [FILES]\n"
"Computes and verifies %s hashes\n"
"\n"
"OPTIONS:\n"
"        -b, --binary    Read files in binary mode.\n"
"        -t, --text      Read files in text mode (default).\n"
"        -T  --tag       Print a BSD-style tag\n"
"        -c, --check     Verify checksums from input file.\n"
"\n"
"        -h, --help      Print this message.\n"
"        -v, --version   Print version information.\n"
"\n"
"    The following options must be proceeded by --check\n"
"        -q, --quiet     Don't print 'OK' messages for valid checksums.\n"
"        -s, --strict    Stop processing when encountering an improperly-\n"
"                        formatted checksum line.\n"
"        -w, --warn      Print a warning when encountering an improperly-\n"
"                        formated checksum line.\n"
"        -S, --status    Don't print any messages, just return a status code\n"
"                        indicating success or failure.\n"
"\n"
"FILES:\n"
"    If no file is specified, or file is '-', read from stdin\n"
"\n"
;

const char *VERSION_TEXT =
"%s version 0.1 COPYRIGHT 2014 (c) Scott Hulberg\n"
"\n"
;

int tarichasum_main(int argc, char **argv,
	unsigned int (*hash_stream)(FILE *stream, uint8_t *out, 
	   unsigned int out_length),
	const char *hash_name, unsigned int hash_len, const char *program_name)
{
	int c;
	struct option o[] = {
		{'b', "binary"},
		{'t', "text"},
		{'T', "tag"},
		{'c', "check"},
		{'h', "help"},
		{'v', "version"},
		{'q', "quiet"},
		{'s', "strict"},
		{'w', "warn"},
		{'S', "status"}
	};

	struct program_settings settings = {
		.binary = 0,
		.check = 0,
		.tag = 0,
		.quiet = 0,
		.strict = 0,
		.warn = 0,
		.status =0,
		.hash_name = hash_name,
		.hash_stream = hash_stream,
		.hash_len = hash_len
	};

	struct option_state optstate = option_init(argc, argv, o,
		sizeof(o)/sizeof(struct option));

	int file_count;
	char **files;
	char *stdin_key = "-";

	while ((c = option_parse(&optstate)) != -1)
	{
		switch (c)
		{
			case 'b':
				settings.binary = 1;
				break;
			case 't':
				settings.binary = 0;
				break;
			case 'T':
				settings.tag = 1;
				break;
			case 'c':
				settings.check = 1;
				break;
			case 'h':
				printf(HELP_TEXT, program_name, hash_name);
				return 0;
			case 'v':
				printf(VERSION_TEXT, program_name);
				return 0;
			case 'q':
				settings.quiet = 1;
				break;
			case 's':
				settings.strict = 1;
				break;
			case 'w':
				settings.warn = 1;
				break;
			case 'S':
				settings.status = 1;
				break;
		}
	}
	file_count = argc - optstate.optpos;
	files = argv + optstate.optpos;
	if (file_count == 0)
	{
		files = &stdin_key;
		file_count = 1;
	}
	if (settings.check)
	{
		return process_files(files, file_count, check_files, &settings);
	}
	else
	{
		return process_files(files, file_count, print_hash, &settings);
	}
}
