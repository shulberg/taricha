struct program_settings {
	int binary;
	int check;
	int tag;
	int quiet;
	int strict;
	int warn;
	int status;

	const char *hash_name;
	unsigned int hash_len;

	unsigned int (*hash_stream)(FILE *stream, uint8_t *out,
			unsigned int out_length);
};

int check_files(FILE *stream, const char *checkfile_name,
		struct program_settings *settings);

int print_hash(FILE *stream, const char *filename,
		struct program_settings *settings);

int process_files(char **files, int file_count,
		int (*file_processor) (FILE *stream, const char *filename,
			struct program_settings *settings),
		struct program_settings *settings);
