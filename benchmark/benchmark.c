#include <debug.h>
#include <lib/string.h>
#include <benchmark/benchmark.h>

#define MAX_BYTES 0x4000
#define RESULT_BYTES 0x1000

#define CYCLE_COUNT_REGADDR	0xE0001004
#define CONTROL_REGADDR 0xE0001000 
#define DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR 0xE000EDFC 

#define SYSTICKS_REG 0xE000E018 

static uint32_t *DWT_CYCCNT    = (uint32_t *) CYCLE_COUNT_REGADDR; 
static uint32_t *DWT_CONTROL   = (uint32_t *) CONTROL_REGADDR; 
static uint32_t *SCB_DEMCR     = (uint32_t *) DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR; 

static int cnt_enable = 0;

__attribute__ ((section (".src_pool"))) char src[MAX_BYTES]  = { 'T' };
__attribute__ ((section (".dest_pool"))) char dest[MAX_BYTES] = { 'D' };
__attribute__ ((section (".result_pool"))) uint64_t result[RESULT_BYTES] = { 0 };

void dwt_cfg (void)
{ 
	*SCB_DEMCR = *SCB_DEMCR | 0x01000000; 
    *DWT_CONTROL = *DWT_CONTROL | 1 ; 
     
    if (!cnt_enable) {  
       cnt_enable  = 1;   
       *DWT_CYCCNT = 0;  
	}
}                              

uint32_t *fetch_cyccnt (void)  
{	
	dwt_cfg ();

	return DWT_CYCCNT;
}                              

void reset_cyccnt (void)
{
	*DWT_CYCCNT = 0;
}

uint32_t *fetch_systicks (void)
{
    return (uint32_t *) SYSTICKS_REG;
}

void sleep (int n)
{
	for (int i = 0; i < n; ++i);
}

#pragma GCC optimize ("O0")
void profiler_main (void)
{
	uint32_t start = 0;
	uint32_t end = 0;
	uint32_t offset = 4;
	int n_iteration = 1000;

	for (uint32_t j = 0; j < RESULT_BYTES; ++j) {
		offset = offset + 4;
		start = 0;
		end = 0;
		reset_cyccnt ();	
		
		//sleep (1000);
		
		start = *fetch_cyccnt ();
		for (int i = 0; i < n_iteration; ++i) {
			memcpy (src, dest, offset);
		}
		end = *fetch_cyccnt ();

		result[j] = end - start;

		reset_cyccnt ();	

		//sleep (1000);
		start = 0;
		end = 0;
	}

	dbg_printf (DL_KDB, "Time: %ld\n", end);
}

void benchmark_handler (void)
{
	profiler_main ();
}
