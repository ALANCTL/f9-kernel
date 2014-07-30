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
	int offset;
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

void measure_offset_3bytes (char*, char*);
void measure_offset_7bytes (char*, char*);
void measure_offset_17bytes (char*, char*);
void measure_offset_31bytes (char*, char*);
void measure_offset_67bytes (char*, char*);
void measure_offset_131bytes (char*, char*);
void measure_offset_257bytes (char*, char*);

struct measure_t measure_functions[] = {	
	{
		.offset = 4,	
		.function = measure_offset_4bytes
	},
	{
		.offset = 8,
		.function = measure_offset_8bytes
	},
	{
		.offset = 16,
		.function = measure_offset_16bytes
	},
	{
		.offset = 32,	
		.function = measure_offset_32bytes
	},
	{
		.offset = 64,
		.function = measure_offset_64bytes
	},
	{
		.offset = 128,
		.function = measure_offset_128bytes
	},
	{
		.offset = 256,
		.function = measure_offset_256bytes
	},
	{
		.offset = 512,
		.function = measure_offset_512bytes
	},
	{
		.offset = 3,	
		.function = measure_offset_3bytes
	},
	{
		.offset = 7,
		.function = measure_offset_7bytes
	},
	{
		.offset = 17,
		.function = measure_offset_17bytes
	},
	{
		.offset = 31,	
		.function = measure_offset_31bytes
	},
	{
		.offset = 67,
		.function = measure_offset_67bytes
	},
	{
		.offset = 131,
		.function = measure_offset_131bytes
	},
	{
		.offset = 257,
		.function = measure_offset_257bytes
	},
};

void measure_offset_4bytes (char* src, char* dest)
{
	memcpy (src, dest, 4);
}

void measure_offset_8bytes (char* src, char* dest)
{
	memcpy (src, dest, 8); 
}

void measure_offset_16bytes (char* src, char* dest)
{
	memcpy (src, dest, 16);
}

void measure_offset_32bytes (char* src, char* dest)
{
	memcpy (src, dest, 32);
}

void measure_offset_64bytes (char* src, char* dest)
{
	memcpy (src, dest, 64);
}

void measure_offset_128bytes (char* src, char* dest)
{
	memcpy (src, dest, 128);
}

void measure_offset_256bytes (char* src, char* dest)
{
	memcpy (src, dest, 256);
}

void measure_offset_512bytes (char* src, char* dest)
{
	memcpy (src, dest, 512);
}

void measure_offset_3bytes (char* src, char* dest)
{
	memcpy (src, dest, 3);
}

void measure_offset_7bytes (char* src, char* dest)
{
	memcpy (src, dest, 7); 
}

void measure_offset_17bytes (char* src, char* dest)
{
	memcpy (src, dest, 17);
}

void measure_offset_31bytes (char* src, char* dest)
{
	memcpy (src, dest, 31);
}

void measure_offset_67bytes (char* src, char* dest)
{
	memcpy (src, dest, 67);
}

void measure_offset_131bytes (char* src, char* dest)
{
	memcpy (src, dest, 131);
}

void measure_offset_257bytes (char* src, char* dest)
{
	memcpy (src, dest, 257);
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

void profiler_main (void)
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

	for (int j = 0; j < sizeof (measure_functions) / sizeof (struct measure_t); ++j) {
		reset_cyccnt ();

		sleep (1000);	

		start = *fetch_cyccnt ();

		for (int i = 0; i < exp; ++i) {
			measure_functions[j].function (src, dest);
		}

		end = *fetch_cyccnt ();
		
		latency = (end - start) / exp;	

		dbg_printf (DL_KDB, "%d %ld\n", measure_functions[j].offset, latency);

		reset_cyccnt ();	
	}
}

void benchmark_handler (void)
{
	profiler_main ();
}
