#include <debug.h>
#include <lib/string.h>
#include <benchmark/benchmark.h>

#define MAX_BYTES		8192
#define SYSTICKS_REG 	0xE000E018   

uint32_t *fetch_systicks (void)
{
    return (uint32_t *) SYSTICKS_REG;
}

uint32_t fetch_systicks_consumption (uint32_t start, uint32_t end)
{
	return (start < end) ? (end - start) : (start - end);
}   

void systicks_stream_copy_access_alignment (void)
{
	uint32_t start			= 0;
	uint32_t end			= 0;
	uint64_t delta			= 0;
	int		 base			= 2;
	int		 n_iterations	= base;
	int		 n_power		= 12;

	/*
	 * Init the measure block context
	 */
	char char_block[MAX_BYTES] __attribute__((aligned));
	
	for (int i = 0; i < (MAX_BYTES / 2); ++i) {
        char_block[i] = 'A';
    }

	/*
	 * The measure process  
	 */
	for (int i = 1; i <= n_power; ++i) {
		n_iterations = base << i;

		start = *fetch_systicks ();

		for (int j = 1; j <= n_iterations; j += 4) {
			memcpy (char_block, char_block + n_iterations / 2 - 1, n_iterations / 2);
		}

		end = *fetch_systicks ();

		delta = fetch_systicks_consumption (start, end);

		dbg_printf (DL_KDB, "%d %ld\n", n_iterations / 2, delta);
	}
}

void benchmark_main (void)
{
	systicks_stream_copy_access_alignment ();
}

void benchmark_handler (void)
{
	benchmark_main ();
}
