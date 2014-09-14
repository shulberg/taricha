#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "options.h"

struct option_state option_init(int argc, char **argv,
		const struct option *options, int option_count)
{
	struct option_state state = {
		.optpos=1,
		.grppos=0,
		.argc=argc,
		.argv=argv,
		.options=options,
		.option_count=option_count
	};
	return state;
}

static int option_parse_group(struct option_state *state)
{
	char *argument = state->argv[state->optpos];
	int l = strlen(argument);
	int i;
	state->grppos++;
	for (i=0; i<state->option_count; i++)
	{
		if (argument[state->grppos] == state->options[i].key)
		{
			if (state->grppos == (l-1))
			{
				state->grppos = 0;
				state->optpos++;
			}
			return state->options[i].key;
		}
	}
	fprintf(stderr, "invalid option: %c\n", argument[state->grppos]);
	exit(1);
}

int option_parse(struct option_state *state)
{
	int i;
	char *argument;
	if (state->optpos == state->argc)
		return -1;

	argument = state->argv[state->optpos];
	if (state->grppos > 0)
	{
		return option_parse_group(state);
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
		state->optpos++;
		return -1;
	}
	else if (argument[1] == '-')
	{
		for (i=0; i<state->option_count; i++)
		{
			if (strcmp(state->options[i].name, argument+2) == 0)
			{
				state->optpos++;
				return state->options[i].key;
			}
		}
		fprintf(stderr, "invalid option: %s\n", argument+2);
		exit(1);
	}
	else
	{
		return option_parse_group(state);
	}
	return -1;
}

