#include <sys/stat.h>
#include <errno.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include "processing.h"

#ifdef _WIN32
	#include <io.h>
	#include <fcntl.h>

	#ifndef S_ISDIR
		#define S_ISDIR(x) ((x & S_IFDIR) == S_IFDIR)
	#endif
#endif

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

int print_hash(FILE *stream, const char *filename,
		struct program_settings *settings)
{
	uint8_t hash[64];
	unsigned int hash_len;
	unsigned int i;

	hash_len = settings->hash_stream(stream, hash, sizeof(hash));

	if (settings->tag)
	{
		printf("%s (%s) = ", settings->hash_name, filename);
	}
	for (i=0; i<hash_len; i++)
	{
		printf("%02"PRIx8, hash[i]);
	}
	if (!settings->tag)
	{
		printf(" %c%s\n", (settings->binary) ? '*' : ' ', filename);
	}
	else
	{
		printf("\n");
	}
	return 0;
}

static void parse_hash(const char *hex_hash, uint8_t *hash,
		unsigned int hash_len)
{
	unsigned int i = 0;
	do
	{
		sscanf(hex_hash, "%02"SCNx8, hash+i);
		i++;
		hex_hash+=2;
	}
	while(i<hash_len);
}

static int parse_checksum_line(char *line, char **filename, uint8_t *hash,
		int *binary, struct program_settings *settings)
{
	regex_t checksum_expression;
	regmatch_t matches[4];

	char pattern[64];

	sprintf(pattern, "^([A-Za-z0-9]{%u}) (\\*| )(.*)$",
			settings->hash_len*2);

	regcomp(&checksum_expression, pattern, REG_EXTENDED);

	if (regexec(&checksum_expression, line, 4, matches, 0) == 0)
	{
		//Standard Format
		parse_hash(line, hash, settings->hash_len);

		if (line[matches[2].rm_so] == '*')
		{
			*binary = 1;
		}
		else
		{
			*binary = 0;
		}

		*filename = line + matches[3].rm_so;
	}
	else
	{
		regfree(&checksum_expression);

		sprintf(pattern, "^%s \\((.*)\\) = ([0-9A-Fa-f]{%u})$",
				settings->hash_name, settings->hash_len*2);

		regcomp(&checksum_expression, pattern, REG_EXTENDED);

		if (regexec(&checksum_expression, line, 4, matches, 0) == 0)
		{
			//Tag format
			parse_hash(line+matches[2].rm_so, hash, settings->hash_len);

			line[matches[1].rm_eo] = '\0';
			*filename = line + matches[1].rm_so;

			*binary = settings->binary;
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

int check_files(FILE *stream, const char *checkfile_name,
		struct program_settings *settings)
{
	size_t len;
	char *line;
	uint8_t expected_hash[64], actual_hash[64];
	int binary;

	uint64_t bad_checksum_count = 0;
	uint64_t bad_line_count = 0;

	char *filename;

	int status_code = 0;

	while ((len = read_line(stream, &line)) > 0)
	{
		if (parse_checksum_line(line, &filename, expected_hash, &binary,
					settings) > -1)
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
			settings->hash_stream(f, actual_hash, settings->hash_len);

			if (memcmp(expected_hash, actual_hash, settings->hash_len) == 0)
			{
				if (!settings->quiet && !settings->status)
				{
					printf("%s: OK\n", filename);
				}
			}
			else
			{
				status_code = 1;
				bad_checksum_count++;

				if (!settings->status)
				{
					printf("%s: FAILED\n", filename);
				}
			}
			fclose(f);
		}
		else
		{
			bad_line_count++;
			if (settings->warn || settings->strict)
			{
				fprintf(stderr, "Invalid checksum line encountered.\n");
			}

			if (settings->strict)
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
	return status_code;
}

int process_files(char **files, int file_count,
		int (*file_processor) (FILE *stream, const char *filename,
			struct program_settings *settings),
		struct program_settings *settings)
{
	int status_code = 0;
	int i;
	for (i=0; i<file_count; i++)
	{
		struct stat file_stat;
		char *filename = files[i];
		if (strcmp(filename,"-") == 0)
		{

			if (settings->binary)
			{
#if defined(_WIN32)
				if(_setmode(_fileno(stdin), _O_BINARY) == -1)
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

			status_code |= file_processor(stdin, "-", settings);
		}
		else if (stat(filename, &file_stat) == 0)
		{
			if (!S_ISDIR(file_stat.st_mode))
			{
				FILE *f;
				if (settings->binary)
				{
					f = fopen(filename, "rb");
				}
				else
				{
					f = fopen(filename, "r");
				}

				if (f != NULL)
				{
					status_code |= file_processor(f, filename, settings);

					fclose(f);
				}
				else
				{
					perror(filename);
					status_code = 1;
				}
			}
			else
			{
				errno = EISDIR;
				perror(filename);
				status_code = 1;
			}
		}
		else
		{
			perror(filename);
			status_code = 1;
		}
	}
	return status_code;
}

