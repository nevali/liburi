#ifdef HAVE_CONFIG_H
# include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SHOW_PREFIX                     1
#define SHOW_EXEC_PREFIX                2
#define SHOW_CFLAGS                     3
#define SHOW_LIBS                       4
#define SHOW_VERSION                    5

static const char *short_program_name = "liburi-config";
static void parseargs(int argc, char **argv);
static void usage(void);
static int willshow(int);

#define SHOW_MAX                        16

static int show[SHOW_MAX];
static int showcount = 0;

int
main(int argc, char **argv)
{
	int c;

	parseargs(argc, argv);

	for(c = 0; c < showcount; c++)
	{
		switch(show[c])
		{
		case SHOW_PREFIX:
			puts(PREFIX);
			break;
		case SHOW_EXEC_PREFIX:
			puts(EXEC_PREFIX);
			break;
		case SHOW_CFLAGS:
			printf("-I%s\n", INCLUDEDIR);
			break;
		case SHOW_LIBS:
			printf("-L%s -luri\n", LIBDIR);
			break;
		case SHOW_VERSION:
			puts(VERSION);
			break;
		}
	}
	return 0;
}

static void
parseargs(int argc, char **argv)
{
	const char *t;
	int ch;

	t = strrchr(argv[0], '/');
	if(t)
	{
		t++;
	}
	else
	{
		t = argv[0];
	}
	short_program_name = t;
	while((ch = getopt(argc, argv, "hv-:")) != -1)
	{
		switch(ch)
		{
		case 'h':
			usage();
			exit(EXIT_SUCCESS);
		case 'v':
			willshow(SHOW_VERSION);
			break;
		case '-':
			if(!strcmp(optarg, "help"))
			{
				usage();
				exit(EXIT_SUCCESS);
			}
			if(!strcmp(optarg, "prefix"))
			{
				willshow(SHOW_PREFIX);
				continue;
			}
			if(!strcmp(optarg, "exec-prefix") || !strcmp(optarg, "exec_prefix"))
			{
				willshow(SHOW_EXEC_PREFIX);
				continue;
			}
			if(!strcmp(optarg, "cflags"))
			{
				willshow(SHOW_CFLAGS);
				continue;
			}
			if(!strcmp(optarg, "libs"))
			{
				willshow(SHOW_LIBS);
				continue;
			}
			if(!strcmp(optarg, "version"))
			{
				willshow(SHOW_VERSION);
				continue;
			}
			fprintf(stderr, "%s: illegal option -- %s\n", argv[0], optarg);
			/* Fallthrough */
		default:
			usage();
			exit(EXIT_FAILURE);
		}
	}
	if(!showcount)
	{
		usage();
		exit(EXIT_SUCCESS);
	}
}

static void
usage(void)
{
	fprintf(stderr, "Print the liburi build configuration\n");
	fprintf(stderr, "Usage: %s OPTIONS\n\n", short_program_name);
	fprintf(stderr, "OPTIONS is one or more of:\n"
			"  -h, --help          Display this usage message and exit\n"
			"  -v, --version       Print the liburi version\n"
			"  --prefix            Print the installation prefix\n"
			"  --exec_prefix       Print the platform-specific installation prefix\n"
			"  --cflags            Print the C compiler flags needed to use liburi\n"
			"  --libs              Print the linker flags needed to use liburi\n");
}

static int
willshow(int what)
{
	int c;

	if(showcount >= SHOW_MAX)
	{
		return -1;
	}
	for(c = 0; c < showcount; c++)
	{
		if(show[c] == what)
		{
			return 0;
		}
	}
	show[showcount] = what;
	showcount++;
	return 0;
}
