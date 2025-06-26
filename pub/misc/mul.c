#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <ctype.h>
#include <unistd.h>

void do_ver(void)
{
 printf("Frequency multiplier v1.0 by sekio Apr 8 2023\n");
}

void do_help(char *name)
{
 printf("usage: %s [-f freq] [-m mul] [-v] [-h/?]\n",name);
 printf("\t -f freq: base frequency in hertz\n");
 printf("\t -m mul: number of multiples\n");
 printf("\t -v: displays version\n");
 printf("\t -h: displays this help\n");

}


int main(int argc, char **argv)
{
    int             c;
    unsigned long   freq = 6925, mul = 5, i;

    while ((c = getopt(argc, argv, "f:m:vh")) != -1)
	switch (c) {
	case 'h':
	    do_help(argv[0]);
	    exit(0);
	case 'v':
	    do_ver();
	    exit(0);
	case 'f':
	    freq = strtoul(optarg, NULL, 10);
	    if ((errno == ERANGE) || (freq == 0)) {
		fprintf(stderr,"Couldn't make sense of \"%s\".\n", optarg);
		exit(1);
	    }
	    break;
	case 'm':
	    mul = strtoul(optarg, NULL, 10);
	    if ((errno == ERANGE) || (mul == 0)) {
		fprintf(stderr,"Couldn't make sense of \"%s\".\n", optarg);
		exit(1);
	    }
	    break;
	 case '?':
            if ((optopt == 'f') || (optopt == 'm'))
	    fprintf(stderr, "Option requires a numeric argument: \"-%c\"\n", optopt);
	    else if (isprint(optopt))
	    fprintf(stderr, "Unknown option: \"-%c\"\n", optopt);
	    else fprintf(stderr, "Unknown option character 0x%x\n", optopt);
	  default:
	    exit(1);
	}

    printf("Number of multiples is %lu\n", mul);
    printf("Base frequency is %lu Hz\n", freq);

    for (i = 1; i <= mul; i++) {
	printf(" x %3lu = %8lu Hz\n", i, i * freq);
    }

    return 0;
}
