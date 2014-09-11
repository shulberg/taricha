#include <sys/stat.h>
#include <errno.h>
#include <regex.h>
#include <string.h>
#include <stdio.h>
#include <inttypes.h>
#include "taricha.h"

#ifdef _WIN32
#include <io.h>
#endif

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

struct option
{
	char key;
	char *name;
};

int optpos = 1;
int grppos = 0;
int opterr = 0;

static int option_parse_group(const char *argument,
		const struct option *options, int option_count)
{
	int l = strlen(argument);
	int i;
	grppos++;
	for (i=0; i<option_count; i++)
	{
		if (argument[grppos] == options[i].key)
		{
			if (grppos == (l-1))
			{
				grppos = 0;
				optpos++;
			}
			return options[i].key;
		}
	}
	fprintf(stderr, "invalid option: %c\n", argument[grppos]);
	exit(1);
}

static int option_parse(int argc, char **argv,
		const struct option *options, int option_count)
{
	int i;
	char *argument;
	if (optpos == argc)
		return -1;

	argument = argv[optpos];
	if (grppos > 0)
	{
		return option_parse_group(argument, options, option_count);
	}

	if (argument[0] != '-')
	{
		return -1;
	}
	else if (strcmp(argument,"-") == 0)
	{
		return -1;
	}
	else if (strcmp(argument,"--") == 0)
	{
		optpos++;
		return -1;
	}
	else if (argument[1] == '-')
	{
		for (i=0; i<option_count; i++)
		{
			if (strcmp(options[i].name, argument+2) == 0)
			{
				optpos++;
				return options[i].key;
			}
		}
		fprintf(stderr, "invalid option: %s\n", argument+2);
		exit(1);
	}
	else
	{
		return option_parse_group(argument, options, option_count);
	}
	return -1;
}

static size_t read_line(FILE * stream, char **line_out)
{
	size_t len = 128; //initial allocation will be double this value
	size_t pos = 0, chars_read;

	char *line = NULL;

	do
	{
		len *= 2;
		line = (char *)realloc(line, len);

		if (fgets(line+pos, len-pos, stream) == NULL)
		{
			break;
		}

		chars_read = strlen(line+pos);
		pos += chars_read;
	}
	while (chars_read > 0 && pos == (len-1) && line[pos-1] != '\n');

	if (pos > 0 && line[pos-1] == '\n')
	{
		len = pos-1;
		line[len] = '\0';
	}
	else
	{
		len = pos;
	}

	*line_out = line;

	return len;
}

size_t (*hash_stream)(FILE *stream, uint8_t *out, size_t out_length);

int binary = 0;
int check = 0;
int tag = 0;
int quiet = 0;
int strict = 0;
int warn = 0;
int status = 0;

char *hash_name;
size_t hash_len;

int tarichasum_return_code = 0;

static void print_hash(FILE *stream, const char *filename)
{
	uint8_t hash[64];
	size_t hash_len;
	int i;

	hash_len = hash_stream(stream, hash, sizeof(hash));

	if (tag)
	{
		printf("%s (%s) = ", hash_name, filename);
	}
	for (i=0; i<hash_len; i++)
	{
		printf("%02"PRIx8, hash[i]);
	}
	if (!tag)
	{
		printf(" %c%s\n", (binary) ? '*' : ' ', filename);
	}
	else
	{
		printf("\n");
	}
}

static void parse_hash(const char *hex_hash, uint8_t *hash)
{
	int i = 0;
	do
	{
		sscanf(hex_hash, "%02"SCNx8, hash+i);
		i++;
		hex_hash+=2;
	}
	while(i<hash_len);
}

static int parse_checksum_line(char *line, char **filename, uint8_t *hash,
		int *_binary)
{
	regex_t checksum_expression;
	regmatch_t matches[4];

	char pattern[64];

	sprintf(pattern, "^([A-Za-z0-9]{%zu})\\s+(\\*)?(\\S.*)$", hash_len*2);
	
	regcomp(&checksum_expression, pattern, REG_EXTENDED);

	if (regexec(&checksum_expression, line, 4, matches, 0) == 0)
	{
		//Standard Format
		parse_hash(line, hash);

		if (line[matches[2].rm_so] == '*')
		{
			*_binary = 1;
		}
		else
		{
			*_binary = 0;
		}

		*filename = line + matches[3].rm_so;
	}
	else
	{
		regfree(&checksum_expression);

		sprintf(pattern, "^%s\\s+\\((.+)\\)\\s+=\\s+([0-9A-Fa-f]{%zu})$",
				hash_name, hash_len*2);

		regcomp(&checksum_expression, pattern, REG_EXTENDED);

		if (regexec(&checksum_expression, line, 4, matches, 0) == 0)
		{
			//Tag format
			parse_hash(line+matches[2].rm_so, hash);

			line[matches[1].rm_eo] = '\0';
			*filename = line + matches[1].rm_so;

			*_binary = binary;
		}
		else
		{
			regfree(&checksum_expression);

			return -1;
		}
	}

	regfree(&checksum_expression);

	return 1;
}

static void check_files(FILE *stream, const char *checkfile_name)
{
	size_t len;
	char *line;
	uint8_t expected_hash[64], actual_hash[64];
	int binary;

	uint64_t bad_checksum_count = 0;
	uint64_t bad_line_count = 0;

	char *filename;

	while ((len = read_line(stream, &line)) > 0)
	{
		if (parse_checksum_line(line, &filename, expected_hash, &binary) > -1)
		{
			FILE *f;
			if (binary)
			{
			   f = fopen(filename, "rb");
			}
			else
			{
			   f = fopen(filename, "r");
			}
			hash_stream(f, actual_hash, hash_len);

			if (memcmp(expected_hash, actual_hash, hash_len) == 0)
			{
				if (!quiet && !status)
				{
					printf("%s: OK\n", filename);
				}
			}
			else
			{
				tarichasum_return_code = 1;
				bad_checksum_count++;
				
				if (!status)
				{
					printf("%s: FAILED\n", filename);
				}
			}
			fclose(f);
		}
		else
		{
			bad_line_count++;
			if (warn || strict)
			{
				fprintf(stderr, "Invalid checksum line encountered.\n");
			}

			if (strict)
			{
				exit(1);
			}
		}
		free(line);
	}
	free(line);

	if (bad_line_count > 0)
	{
		fprintf(stderr, "WARNING: %"PRIu64" line%s improperly formatted.\n",
				bad_line_count, ((bad_line_count > 1) ? "s" : ""));
	}

	if (bad_checksum_count > 0)
	{
		fprintf(stderr, "WARNING: %"PRIu64" checksum%s failed.\n",
				bad_checksum_count, ((bad_checksum_count > 1) ? "s" : ""));
	}
}

static void process_files(int argc, char **argv,
		void (*file_processor)(FILE *stream, const char *filename))
{
	if (optpos == argc)
	{
		file_processor(stdin, "-");
	}
	else
	{
		do
		{
			struct stat file_stat;
			char *filename = argv[optpos];
			if (strcmp(filename,"-") == 0)
			{

				if (binary)
				{
#if defined(_WIN32)
					if(_setmode(stdin, O_BINARY) == -1)
					{
						perror("Could not set stdin to binary mode");
						exit(1);
					}
#elif defined(REOPEN_STDIN)
					if (freopen(NULL, "rb", stdin) == NULL)
					{
						perror("Could not reopen stdin in binary mode");
						exit(1);
					}
#endif
				}

				file_processor(stdin, "-");
			}
			else if (stat(filename, &file_stat) == 0)
			{
				if (!S_ISDIR(file_stat.st_mode))
				{
					FILE *f;
					if (binary)
					{
						f = fopen(filename, "rb");
					}
					else
					{
						f = fopen(filename, "r");
					}

					if (f != NULL)
					{
						file_processor(f, filename);

						fclose(f);
					}
					else
					{
						perror(filename);
						tarichasum_return_code = 1;
					}
				}
				else
				{
					errno = EISDIR;
					perror(filename);
					tarichasum_return_code = 1;
				}
			}
			else
			{
				perror(filename);
				tarichasum_return_code = 1;
			}
		}
		while (++optpos < argc);
	}
}

int tarichasum_main(int argc, char **argv,
	size_t (*_hash_stream)(FILE *stream, uint8_t *out, size_t out_length),
	char *program_name, char *_hash_name, size_t _hash_len)
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

	hash_name = _hash_name;
	hash_stream = _hash_stream;
	hash_len = _hash_len;

	while ((c = option_parse(argc, argv, o,
					sizeof(o)/sizeof(struct option))) != -1)
	{
		switch (c)
		{
			case 'b':
				binary = 1;
				break;
			case 't':
				binary = 0;
				break;
			case 'T':
				tag = 1;
				break;
			case 'c':
				check = 1;
				break;
			case 'h':
				printf(HELP_TEXT, program_name, hash_name);
				return 0;
			case 'v':
				printf(VERSION_TEXT, program_name);
				return 0;
			case 'q':
				quiet = 1;
				break;
			case 's':
				strict = 1;
				break;
			case 'w':
				warn = 1;
				break;
			case 'S':
				status = 1;
				break;
		}
	}
	if (check)
	{
		process_files(argc, argv, check_files);
	}
	else
	{
		process_files(argc, argv, print_hash);
	}
	return tarichasum_return_code;
}
