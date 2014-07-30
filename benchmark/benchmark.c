#include <debug.h>
#include <lib/string.h>
#include <benchmark/benchmark.h>

#define MAX_BYTES 512

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

void measure_alignment (void)
{
	uint32_t start	 = 0;
	uint32_t end	 = 0;
	uint64_t delta	 = 0;

	/*
	 * Init the measure block context
	 */
	char src[MAX_BYTES] __attribute__((aligned));
	char dest[MAX_BYTES] __attribute__((aligned));

	for (int i = 0; i < MAX_BYTES; ++i) {
        src[i] = 'A';
    }

	/*
	 * The measure process  
	 */
}

void measure_unalignment (void)
{
	uint32_t start	 = 0;
	uint32_t end	 = 0;
	uint64_t delta	 = 0;

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

		start = *fetch_cyccnt ();

		memcpy (char_block, char_block + offset - 1, offset);

		end = *fetch_cyccnt ();

		delta = end - start;

		dbg_printf (DL_KDB, "%ld\n", delta);
	}
}

void benchmark_main (void)
{
	dbg_printf (DL_KDB, "alignment\n");

	measure_alignment ();
}

void benchmark_handler (void)
{
	benchmark_main ();
}
