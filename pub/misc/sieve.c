#include <stdio.h>
#include <stdint.h>
#include <limits.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <math.h>

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define BANNER "sieve of Eratosthenes by sekio\n"

#define DEFAULT_SIEVE_SIZE 	(uint64_t)(INT_MAX*1.33)
#define SIEVE_SIZE_CEILING 	(uint64_t)3000000000
#define SIEVE_SIZE_MIN 		(uint64_t)2
#define DEFAULT_ROUNDS_PRINT 	(uint64_t)1000000

#define PRIME_TABLE_FUDGE_MUL 	1.2
#define PRIME_TABLE_FUDGE_ADD	3
// fill sieve table
// we start at 2 to avoid counting 1 as prime
void init_sieve_table(uint64_t* table, uint64_t size) {
 uint64_t i;
 for (i = 0; i<size; i++)
  table[i] = i+2;
}

uint64_t find_first_nonzero_entry(uint64_t start, uint64_t *table,uint64_t size) {
 uint64_t i;
 // scan over table and stop when you find nonzero
 // we don't need to scan the whole table as anything less than the prime
 // we just found is going to be zero anyway
 for (i = start; i<size && table[i]==0; i++);
 if (i >= size) return 0;
 return table[i]; // either some table[i] != 0 or i==size and we return 0
}

void do_sieve_round(uint64_t* table, uint64_t p, uint64_t size) {
  uint64_t i;
  for (i = (p-2); i<size; i+=p) // table starts at 2
   table[i] = 0; // remove multiples of p
}

// formats a nice time
// ctime adds a \n at the end I don't want
char formattime_buf[256];
char* formattime(time_t t) {
 struct tm *tm_local;
 tm_local = localtime(&t);
 strftime(formattime_buf, 256, "%a %x %X %Z", tm_local);
 return strdup(formattime_buf); // yikes
}

int main(int argc, char** argv) {
 uint64_t n=0, p=2, i;
 int64_t sieve_size, primes_size, rounds_print,new_rounds_print;
 uint64_t *sieve_table;
 uint64_t *primes;
 double speed, nsecs;
 time_t time_init, time_now;
 clock_t clock_init, clock_now;
 char * outfilename = "primes.out";
 FILE *outfile;
 int opt;

 printf("%s", BANNER);

 sieve_size = DEFAULT_SIEVE_SIZE;
 rounds_print = DEFAULT_ROUNDS_PRINT;

 while ((opt = getopt(argc, argv, "s:r:o:h?"))!=-1) {
  switch(opt) {
   case 's': // sieve size
    sieve_size = atol(optarg);
    if (sieve_size < 2) {
     fprintf(stderr,"invalid sieve size %ld, setting to minimum of %lu\n", sieve_size, SIEVE_SIZE_MIN);
     sieve_size = SIEVE_SIZE_MIN;
    }
    if (sieve_size > SIEVE_SIZE_CEILING)
     fprintf(stderr, "warning: sieve size %ld is greater than SIEVE_SIZE_CEILING=%lu\n", sieve_size, SIEVE_SIZE_CEILING);
    break;
   case 'r': // rounds to print
    rounds_print = atol(optarg);
    if ((rounds_print < 1) || (rounds_print > sieve_size)) {
     new_rounds_print = min(DEFAULT_ROUNDS_PRINT, sieve_size/100);
     if (new_rounds_print < 1) new_rounds_print = 1;
     fprintf(stderr,"invalid rounds to print %ld, setting to %ld\n",rounds_print,new_rounds_print);
     rounds_print = new_rounds_print;
    }
    break;
   case 'o': // output file
    outfilename = strdup(optarg);
    break;
   // help
   case 'h':
   case '?':
   default:
    printf("usage: %s [-o outfile] [-s sieve_size] [-r print_rounds]\n", argv[0]);
    exit(EXIT_FAILURE);
  }
 }

 // n/log(n) is an approximation of the number of primes less than n
 primes_size = (uint64_t)ceil((sieve_size/log(sieve_size))*PRIME_TABLE_FUDGE_MUL) + PRIME_TABLE_FUDGE_ADD;

 // open output file
 printf("outputting to \"%s\"\n", outfilename);
 outfile = fopen(outfilename,"w");
 if (outfile == NULL) {
  fprintf(stderr,"can't open \"%s\" for writing\n", outfilename);
  abort();
 }

 fputs(BANNER, outfile);

 printf("sieve_size=%li primes_size=%li rounds_print=%li\n", sieve_size, primes_size, rounds_print);
 fprintf(outfile, "sieve_size=%li primes_size=%li rounds_print=%li\n", sieve_size, primes_size, rounds_print);

 // get initial time
 time_init=time(NULL);
 clock_init = clock();

 // allocate tables
 sieve_table = malloc(sieve_size*sizeof(uint64_t));
 primes = calloc(primes_size, sizeof(uint64_t));
 if (sieve_table == NULL) {
  fprintf(stderr, "can't allocate sieve table, size %lu\n", sieve_size*sizeof(uint64_t));
  abort();
 }
 if (primes == NULL) {
  fprintf(stderr, "can't allocate primes table, size %lu\n", primes_size*sizeof(uint64_t));
  abort();
 }


 // fill sieve table
 printf("[%s] build sieve table\n", formattime(time_init));
 init_sieve_table(sieve_table,sieve_size);

 printf("[%s] start sieving process\n", formattime(time_init));
 p=1;
 while (p = find_first_nonzero_entry(p-1,sieve_table,sieve_size)) {
  // p is now a presumed prime

  // print status
  if (((n) % rounds_print) == 0) {
   time_now=time(NULL);
   clock_now = clock();
   speed=n/((double)(clock_now-clock_init)/CLOCKS_PER_SEC);
   printf("[%s] (%6.2f%%) n=%6lu p=%8lu speed=%7.1f rds/s\n",formattime(time_now),
							 ((p/(double)(sieve_size+1))*100.0),n,
							 p,speed);
   fflush(stdout); // needed for compatibility with tee, less, etc
  }

  if (n > primes_size) { // should not happen
   fputs("exceeded prime table space, increase PRIME_TABLE_FUDGE_MUL/ADD",stderr);
   fprintf(stderr,"n=%lu primes_size=%li\n", n, primes_size);
   abort();
  }

  // add p to primes table
  primes[n++] = p;
  // remove multiples of p from sieve table
  do_sieve_round(sieve_table, p, sieve_size);
 }
 // final speed measurement
 time_now = time(NULL);

 clock_now = clock();
 nsecs=((double)(clock_now-clock_init)/CLOCKS_PER_SEC);
 speed = (double)n/nsecs;

 printf("found %lu primes (%.2f%% of prime table space)\n",n, ((double)n/primes_size)*100.0f);
 printf("start time %s\nfinish time %s\n%.3f seconds\n%.2f rounds/s\n",formattime(time_init),formattime(time_now),nsecs,speed);

 printf("Writing to output file %s ... ",outfilename);
 fflush(stdout);
 // do output to file
 fprintf(outfile,"found %lu primes (%.2f%% of prime table space)\n",n, ((double)n/primes_size)*100.0f);
 fprintf(outfile,"start time %s\nfinish time %s\n%.2f seconds\n%.2f rounds/s\n",formattime(time_init),formattime(time_now),nsecs,speed);
 fprintf(outfile,"-------- LIST OF PRIMES --------\n");
 for (i=0; i<n; i++) {
  fprintf(outfile,"no.%lu: %lu\n", i+1, primes[i]);
 }
 printf("done!\n");
 // close files
 fclose(outfile);

 // free allocated space
 free(sieve_table);
 free(primes);

 return 0;
}
