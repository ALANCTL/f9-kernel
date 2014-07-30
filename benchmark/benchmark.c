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

typedef void (*memcpy_function_t)(char*, char*);

struct measure_t {
	memcpy_function_t function;
};

void measure_offset_4bytes (char*, char*);
void measure_offset_8bytes (char*, char*);
void measure_offset_16bytes (char*, char*);
void measure_offset_32bytes (char*, char*);
void measure_offset_64bytes (char*, char*);
void measure_offset_128bytes (char*, char*);
void measure_offset_256bytes (char*, char*);
void measure_offset_512bytes (char*, char*);

struct measure_t measure_functions[] = {
	{
		.function = measure_offset_4bytes
	},
	{
		.function = measure_offset_8bytes
	},
	{
		.function = measure_offset_16bytes
	},
	{
		.function = measure_offset_32bytes
	},
	{
		.function = measure_offset_64bytes
	},
	{
		.function = measure_offset_128bytes
	},
	{
		.function = measure_offset_256bytes
	},
	{
		.function = measure_offset_512bytes
	}
};

void measure_offset_4bytes (char* src, char* dest)
{
	memcpy (src, dest, 4);
}

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

void measure_alignment (void)
{
	uint32_t start	 = 0;
	uint32_t end	 = 0;
	uint64_t latency = 0;

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
	int exp = 1000;

	reset_cyccnt ();

	sleep (1000);	

	start = *fetch_cyccnt ();

	for (int i = 0; i < exp; ++i) {
		memcpy (src, dest, MAX_BYTES);  
	}

	end = *fetch_cyccnt ();
		
	latency = (end - start) / exp;	

	dbg_printf (DL_KDB, "%ld\n", latency);

	dbg_printf (DL_KDB, "%ld\n", *DWT_CYCCNT);
	
	reset_cyccnt ();	

	dbg_printf (DL_KDB, "%ld\n", *DWT_CYCCNT);

	sleep (1000);

	dbg_printf (DL_KDB, "%ld\n", *DWT_CYCCNT);
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
