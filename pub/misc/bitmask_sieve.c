#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include <getopt.h>
#include <math.h>
#include <time.h>

#ifndef max
#define max(a,b) (((a) (b)) ? (a) : (b))
#endif
#ifndef min
#define min(a,b) (((a) < (b)) ? (a) : (b))
#endif

#define BITMASK_NOTFOUND  UINT64_MAX
#define MAX_ROUNDS_PRINT 10000000
#define u32 uint32_t
#define u64 uint64_t
#define s32 int32_t
#define s64 int64_t

// default sieve size (in 32bit ints)
#define SIEVE_SIZE_DEF INT32_MAX/16

// find nonzero entry in bitmask, return index
u64 bitmask_find(u64 start, size_t field_size, u32 *field) {
 u32 x;
 u64 r, i, start_i;

 // convert bit index to table index
 start_i = start/32;
#ifdef DEBUG
 fprintf(stderr, "start r=%lu i=%lu\n", start, start_i);
#endif

 // scan bitmask and skip zeros
 for (i = start_i; (i < field_size) && (field[i] == 0); i++) {
#ifdef DEBUG
  fprintf(stderr, "bmf: zero field[%lu] = 0x%08X\n", i, field[i]);
#endif
 };
 // return error if we find no bits present
 if (i == field_size) {
#ifdef DEBUG
  fprintf(stderr, "bmf: all fields zero\n");
#endif
  return UINT64_MAX;
 }
#ifdef DEBUG
 fprintf(stderr, "bmf: nonzero field[%lu] = 0x%08X\n", i, field[i]);
#endif

 // scan int field for bit set
 for (x = 0; (x < 32) && (((field[i] >> x) &1) == 0); x++) {
#ifdef DEBUG
  fprintf(stderr, "bmf: x=%u field[%lu] = %u field>>x = %u, %i\n", x, i, field[i], field[i] >> x, (field[i] >> x) & 1);
#endif
 };

 // should not happen
 // if (x == 32) { printf("x exceeds field size\n"); abort(); }

 // convert bit position + table position to bit index
 r = (i * 32) + x;

#ifdef DEBUG
 fprintf(stderr, "bitmask_find i = %lu x = %u r = %lu\n", i, x, r);
#endif

 return r;
}

// set a bit index to zero
void bitmask_remove_index(u64 r, size_t field_size, u32 *field) {
 u64 i;
 u32 x, mask;
 // convert bit index to table index + bit offset
 x = r % 32;
 i = r / 32;

 // should not happen
 if (i > field_size) {
    fprintf(stderr, "error: bitmask_remove_index outside field_size\n");
    abort();
 }

 // mask bit out
 mask = ~(1 << x);
 field[i] &= mask;

#ifdef DEBUG
 fprintf(stderr, "bitmask_remove_index mask=0x%08x i=%lu x=%u r=%lu\n", mask, i, x, r);
#endif
}

// hackery around asctime()
void gettime(char * timebuf){
 time_t now;
 struct tm now_tm;
 now = time(NULL);
 localtime_r(&now, &now_tm);
 asctime_r(&now_tm, timebuf);
 timebuf[strlen(timebuf)-1]=0;
}

// debugging tool
#ifdef DEBUG
void dumpmask(size_t field_size, u32 *field) {
 u32 i;
 for(i=0;i<field_size;i++) {
  dprintf("field[%d] = 0x%08X\t",i,field[i]);
  if ((i % 3) == 2) dprintf("\n");
 }
 dprintf("\n");
}
#endif

int main(int argc, char **argv){
 u64 r, n, i, old_n;
 size_t mask_size = SIEVE_SIZE_DEF;
 s64 mask_size_i, rounds_print_i;
 u32 rounds_print = 1, rounds_printed, *mask, time_h, time_m, time_s;
 int opt;
 char timebuf[80];
 clock_t clock_start, clock_last, clock_now;
 time_t time_start, time_end;
 double rounds_per_sec, rounds_per_sec_overall, rounds_per_sec_max, time_elapsed;
 FILE *outfile = stdout;

 clock_start = clock();
 clock_now = clock(); //stop whining about uninitialized vars
 time_start = time(NULL);

 fprintf(stderr, "improved bitfield sieve by sekio\n");

 while ((opt = getopt(argc, argv, "s:r:o:h?"))!=-1) {
  switch(opt) {
   case 's': // sieve size
    mask_size_i = atol(optarg);
    if (mask_size_i < 0) mask_size_i = 0 - mask_size_i;
    if (mask_size_i < 8) mask_size_i = 8;
    fprintf(stderr, "sieve size set to: %li\n", mask_size_i);
    mask_size = mask_size_i;
    break;

   case 'r': // rounds to print
    rounds_print_i = atol(optarg);
    if (rounds_print_i < 0) rounds_print_i = 0 - rounds_print_i;
    if (rounds_print_i < 1) rounds_print_i = 1;
    fprintf(stderr, "rounds print set to: %li\n", rounds_print_i);
    rounds_print = rounds_print_i;
    break;

   case 'o': // output file
    fprintf(stderr, "output file set to '%s'\n", optarg);
    outfile = fopen(optarg, "w");
    if (outfile == NULL) {
      fprintf(stderr, "can't open outfile: %s\n", optarg);
      outfile = stdout;
    }
    break;

   default:
    fprintf(stderr, "usage: %s [-s sievesize] [-r rounds_print] [-o outfilename]", argv[0]);
    exit(1);
    break;
  }
 }

 // allocate and initialize sieve
 fprintf(stderr, "sieve size=%lu bits (%lu entries x %lu bits)\n", (u64)mask_size * 32, mask_size, sizeof(u32)*8);
 fprintf(stderr, "allocating %lu byte sieve... ", mask_size * sizeof(u32));
 mask = malloc(mask_size * sizeof(u32));
 fprintf(stderr, "initializing sieve... ");
 memset(mask, 0xff, mask_size * sizeof(u32));

 // remove obvious non primes
 bitmask_remove_index(0, mask_size, mask);
 bitmask_remove_index(1, mask_size, mask);

 r=0;
 n=1;

 rounds_printed = 0;
 rounds_per_sec_max = 0;

 clock_last = clock();

 gettime(timebuf);
 fprintf(stderr, "ok!\n[%s] start run\n", timebuf);
 fflush(stderr);

 do {
  r++;
  old_n = n;

#ifdef DEBUG
  dumpmask(mask_size, mask);
#endif

  n=bitmask_find(n, mask_size, mask);
 // if (n == old_n) {
 //  abort();
 // }
  if (n != BITMASK_NOTFOUND) {
   fprintf(outfile, "%lu\n", n);

   for (i = n; i < mask_size * 32; i += n)
    bitmask_remove_index(i, mask_size, mask);

   if ((r % rounds_print) == 0) {
    // flush output file
    fflush(outfile);

    // timing calculations
    gettime(timebuf);
    clock_last = clock_now;
    clock_now = clock();

    // rounds/sec between last printed output and now
    rounds_per_sec = rounds_print / ((double)(clock_now - clock_last) / CLOCKS_PER_SEC);

    // overall rounds/sec since start
    rounds_per_sec_overall = r / ((double)(clock_now - clock_start) / CLOCKS_PER_SEC);

    // record peak rounds/s
    if (rounds_per_sec > rounds_per_sec_max) rounds_per_sec_max = rounds_per_sec;

    fprintf(stderr, "[%s %.4f%%] r=%lu n=%lu spd=%.4f ova=%.2f\n", timebuf, 100.0f*((double)n/(mask_size*32)), r, n, rounds_per_sec, rounds_per_sec_overall);

    // dynamic output frequency scaling
    if((++rounds_printed >= 20) && (rounds_print < MAX_ROUNDS_PRINT)) {
     rounds_print *= 10;
     rounds_printed = 0;
    };

    fflush(stdout);
   }

  }
 } while (n != BITMASK_NOTFOUND);

 fprintf(stderr, "\a\a\a!!! Done !!!\nFree mask...");
 free(mask);
 fprintf(stderr, " done.\n");

 // final timing calculation
 clock_now = clock();

 rounds_per_sec = r / ((double)(clock_now - clock_start) / CLOCKS_PER_SEC);

 time_end = time(NULL);
 time_elapsed = difftime(time_end, time_start);
 time_h = (u32)time_elapsed/3600;
 time_elapsed -= time_h * 3600;
 time_m = (u32)time_elapsed/60;
 time_elapsed -= time_m * 60;
 time_s = (u32)time_elapsed;
 fprintf(stderr,"Final time %u:%02u:%02u\n", time_h, time_m, time_s);
 fprintf(stderr,"Found %lu primes. Final prime=%lu\nSpeed (rounds/sec)=%.3f Peak=%.3f\n", r, old_n, rounds_per_sec, rounds_per_sec_max);

 if (outfile != stdout)
  fclose(outfile);


 return 0;
}
