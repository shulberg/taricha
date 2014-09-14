
struct option
{
	char key;
	char *name;
};

struct option_state
{
	int optpos;
	int grppos;
	
	int argc;
	char **argv;
	
	const struct option *options;
	int option_count;
};

struct option_state option_init(int argc, char **argv,
		const struct option *options, int option_count);

int option_parse(struct option_state *state);
