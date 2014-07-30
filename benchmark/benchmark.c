#include <debug.h>
#include <lib/string.h>
#include <benchmark/benchmark.h>

#define MAX_BYTES 4096

#define CYCLE_COUNT_REGADDR	0xE0001004
#define CONTROL_REGADDR 0xE0001000 
#define DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR 0xE000EDFC 

static uint32_t *DWT_CYCCNT    = (uint32_t *) CYCLE_COUNT_REGADDR; 
static uint32_t *DWT_CONTROL   = (uint32_t *) CONTROL_REGADDR; 
static uint32_t *SCB_DEMCR     = (uint32_t *) DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR; 

static int cnt_enable = 0;

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

void sleep (int n)
{
	for (int i = 0; i < n; ++i);
}

void profiler_main (void)
{
	uint32_t start	 = 0;
	uint32_t end	 = 0;
	uint64_t latency = 0;
	
	/*
	 * Init the measure block context
	 */
	char src[MAX_BYTES] __attribute__((aligned (4)));
	char dest[MAX_BYTES] __attribute__((aligned (4)));
}

void print_latency (void)
{
}

void benchmark_handler (void)
{
	profiler_main ();
}
