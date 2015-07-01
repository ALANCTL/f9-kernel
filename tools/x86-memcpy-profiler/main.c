#include <stdio.h>
#include <string.h>
#include <time.h>
#include <math.h>

#define RANDOM_EXPERIMENTS 	10000
#define BLK_4				4
#define BLK_OFFSET			20
#define BLK_UPPER_BOUND		16777216

static struct timespec start_time;
static struct timespec end_time;
static double elapsed_time = 0;
static double avg_time = 0;

static volatile char* __attribute__((aligned)) src;
static volatile char* __attribute__((aligned)) dest;

typedef void ms_func(void);

ms_func* memcpy_profiler(void);

typedef struct {
	const char* fname;
	ms_func* signature;
	const char* manual;
} ms_list;

void aligned_case(void);
void unaligned_case(void);

#define MKCL(n, d) {.fname=#n, .signature=n ## _case, .manual=d}

ms_list ml[] = {
	MKCL(aligned, "Measure the memcpy of alignment case."),
	MKCL(unaligned, "Measure the memcpy of non-alignment case."),
};

__attribute__ ((constructor)) void src_init() {
	src = malloc (sizeof(char) * BLK_UPPER_BOUND);

	int i = 0;
	while(i++ < BLK_UPPER_BOUND) {
		src[i] = 'A' + (rand() % 26);
	}
}

__attribute__ ((constructor)) void dest_init() {
	dest = malloc (sizeof(char) * BLK_UPPER_BOUND);

	int i = 0;
	while(i++ < BLK_UPPER_BOUND) {
		dest[i] = 'a' + (rand() % 26);
	}
}

void dbg_printf(const char* msg) {
	printf("%s\n", msg);
}


void aligned_case(void) {
	dbg_printf("start to measure the alignment case.");

	int i, j;
	FILE *fopt = fopen("aligned_case.txt", "w");

	for(i = 0; i < BLK_OFFSET; ++i) {
		for(j = 0; j < RANDOM_EXPERIMENTS; ++j) {
			clock_gettime(CLOCK_REALTIME, &start_time);

			memcpy(dest, src, BLK_4 << i);
		
			clock_gettime(CLOCK_REALTIME, &end_time);
		
			elapsed_time = end_time.tv_nsec - start_time.tv_nsec;
		}		

		avg_time = elapsed_time / RANDOM_EXPERIMENTS;
		
		fprintf(fopt, "%d %lf\n", BLK_4 << i, avg_time);
	}

	dbg_printf("stop to measure the alignment case.");
}

void unaligned_case(void) {
	dbg_printf("start to measure the unalignment case.");

	int i, j;
	FILE *fopt	= fopen("unaligned_case.txt", "w");

	for(i = 0; i < BLK_OFFSET; ++i) {
		for(j = 0; j < RANDOM_EXPERIMENTS; ++j) {
			clock_gettime(CLOCK_REALTIME, &start_time);

			memcpy(dest + 1, src + 2, BLK_4 << i);
		
			clock_gettime(CLOCK_REALTIME, &end_time);
		
			elapsed_time = end_time.tv_nsec - start_time.tv_nsec;
		}		

		avg_time = elapsed_time / RANDOM_EXPERIMENTS;
		
		fprintf(fopt, "%d %lf\n", BLK_4 << i, avg_time);
	}

	dbg_printf("stop to measure the unalignment case.");
}

ms_func* memcpy_profiler(void) {
	int i;
	
	for(i = 0; i < sizeof(ml) / sizeof(ml[0]); ++i) {
		ml[i].signature();
	}

	return NULL;
}

int main(void)
{
	memcpy_profiler();

	return 0;
}

__attribute__((destructor)) void src_free() {
	free(src);
}

__attribute__((destructor)) void dest_free() {
	free(dest);
}
