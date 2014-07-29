#include <debug.h>
#include <lib/string.h>
#include <benchmark/benchmark.h>

#define MAX_BYTES		256
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
	uint32_t start	 = 0;
	uint32_t end	 = 0;
	uint64_t delta	 = 0;
	uint64_t latency = 0;	

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
	for (uint32_t i = 1, offset = 4; i <= ((MAX_BYTES / 2) / 4); ++i, offset += 4) {
		start = *fetch_systicks ();

		memcpy (char_block, char_block + offset - 1, offset);

		end = *fetch_systicks ();

		delta = fetch_systicks_consumption (start, end);

		dbg_printf (DL_KDB, "%ld\n", delta);
	}
}

void systicks_stream_copy_access_unalignment (void)
{
	uint32_t start	 = 0;
	uint32_t end	 = 0;
	uint64_t delta	 = 0;
	uint64_t latency = 0;	

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
	for (uint32_t i = 1, offset = 3; i <= ((MAX_BYTES / 2) / 3) && offset <= 128; ++i, offset += 3) {
		if (offset % 4 == 0)
			offset += 1;

		start = *fetch_systicks ();

		memcpy (char_block, char_block + offset - 1, offset);

		end = *fetch_systicks ();

		delta = fetch_systicks_consumption (start, end);

		dbg_printf (DL_KDB, "%ld\n", delta);
	}
}

void benchmark_main (void)
{
	systicks_stream_copy_access_unalignment ();
}

void benchmark_handler (void)
{
	benchmark_main ();
}
