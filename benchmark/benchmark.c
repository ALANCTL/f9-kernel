#include INC_PLAT(systick.h)

#include <debug.h>
#include <lib/string.h>
#include <platform/irq.h>
#include <benchmark/benchmark.h>

#define MAX_BYTES		0x4000
#define RESULT_BYTES	0x1000

__attribute__ ((section (".src_pool"))) char src[MAX_BYTES]  = { 'T' };
__attribute__ ((section (".dest_pool"))) char dest[MAX_BYTES] = { 'D' };
__attribute__ ((section (".result_pool"))) uint64_t result[RESULT_BYTES] = { 0 };

#pragma GCC optimize ("O0")
void profiler_main (void)
{
	uint32_t start = 0;
	uint32_t end = 0;
	uint32_t offset = 4;
	int n_iteration = 1000;

	*SYSTICK_CTL = 0x00000007;
	init_systick (CONFIG_KTIMER_HEARTBEAT, 0);
	
	for (uint32_t j = 0; j < RESULT_BYTES; ++j) {
		offset = offset + 4;
		
		start = *SYSTICK_VAL;

		for (int i = 0; i < n_iteration; ++i) {
			memcpy (src, dest, offset);
		}

		end = *SYSTICK_VAL;

		result[j] = start - end;
	}
    
	dbg_printf (DL_KDB, "The Measurement has finished.\n");
}

void benchmark_handler (void)
{
	profiler_main ();
}
