#include <stdio.h>
#include <stdint.h>
#include <string.h>

#define POOL_SIZE	0x1B
#define ALIGN_NUM	(POOL_SIZE / 0x4)

__attribute__ ((section (".src_pool"))) char src[POOL_SIZE];
__attribute__ ((section (".dest_pool"))) char dest[POOL_SIZE];

uint32_t src_aligned[ALIGN_NUM];
uint32_t dest_aligned[ALIGN_NUM];

void init_array (char* foo, char seed)
{
	for (int i = 0; i < POOL_SIZE; ++i) {
		foo[i] = seed;
	}
}

void fetch_aligned (char* foo, uint32_t* bar)
{
	uint32_t tmp = 0x0;
	
	for (int i = 0, j = 0; i < POOL_SIZE; ++i) {
		tmp = (uint32_t) foo + i;

		if (tmp % 0x4 == 0) {
			bar[j++] = tmp;
		}
	}
}

void print_address (uint32_t* bar)
{
	for (int i = 0; i < ALIGN_NUM; ++i) {
		printf ("%p\n", bar[i]);
	}

	printf ("\n");
}

void test_memcpy_aligned ()
{
	for (int i = 0; i < ALIGN_NUM; ++i) {
		init_array (dest_aligned[i], 'x');

		//printf ("src : %s\ndest: %s\noffset: %d\n\n", src_aligned[i], dest_aligned[i], src + POOL_SIZE - src_aligned[i] + 1);
		
		memcpy ((char *)dest_aligned[i], (char *)src_aligned[i], (uint32_t) src + POOL_SIZE - src_aligned[i] + 1);

		init_array (dest_aligned[i], 'a');
	}
}

int main (int argc, char** argv, char** env)
{	
	init_array (src, 'A');

	fetch_aligned (src, src_aligned);
	fetch_aligned (dest, dest_aligned);

	print_address (src_aligned);
	print_address (dest_aligned);

	test_memcpy_aligned ();

	return 0;
}
