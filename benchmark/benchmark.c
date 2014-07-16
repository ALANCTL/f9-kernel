#include <debug.h>
#include <lib/string.h>
#include <benchmark/benchmark.h>

#define MAX_BSS_BYTES           5440
#define MEASURE_BLOCK_SIZE      1024

#define CYCLE_COUNT_REGADDR                         0xE0001004 
#define CONTROL_REGADDR                             0xE0001000
#define DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR     0xE000EDFC

#define SYSTICKS_REG 								0xE000E018   

typedef void (*benchmark_function_t)(void);

struct benchmark_t {
    char *name;
    benchmark_function_t function;
};

void dwt_stream_copy_access_unalignment (void); 
void dwt_stream_copy_access_alignment (void); 
void systicks_stream_copy_access_unalignment (void); 
void systicks_stream_copy_access_alignment (void); 

void dwt_set_to_zero (void);
void systicks_set_to_zero (void);

static uint64_t *DWT_CYCCNT     = (uint64_t *) CYCLE_COUNT_REGADDR; 
static uint64_t *DWT_CONTROL    = (uint64_t *) CONTROL_REGADDR; 
static uint64_t *SCB_DEMCR      = (uint64_t *) DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR;

static int cnt_enable = 0;  

static char cblock[MEASURE_BLOCK_SIZE];

struct benchmark_t benchmark_functions[] = {
	{
		.name = "DWT SCA unalignment: ",
		.function = dwt_stream_copy_access_unalignment
	},

	{
		.name = "DWT SCA alignment: ",
		.function = dwt_stream_copy_access_alignment
	},

	{	
		.name = "Systicks SCA unalignment: ", 
		.function = systicks_stream_copy_access_unalignment
	},

	{
		.name = "Systicks SCA alignment: ",
		.function = systicks_stream_copy_access_alignment
	},

	{
		.name = "DWT memset: ",
		.function = dwt_set_to_zero
	},

	{
		.name = "Systick memset: ",
		.function = systicks_set_to_zero
	}
};

void dwt_cfg (void) 
{
    *SCB_DEMCR      = *SCB_DEMCR    | 0x01000000;
    *DWT_CONTROL    = *DWT_CONTROL  | 1 ; 

    if (!cnt_enable) {
        cnt_enable  = 1;        
        *DWT_CYCCNT = 0;  
    }   
}

uint64_t *fetch_cyccnt (void)
{
    dwt_cfg (); 

    return DWT_CYCCNT;
}

uint64_t *fetch_systicks (void)
{
    return (uint64_t *) SYSTICKS_REG;
}

uint64_t fetch_systicks_consumption (uint32_t start, uint32_t end)
{
	return (start < end) ? (end - start) : (start - end);
}   

void init_char_block (int block_size)
{
    if (block_size > MEASURE_BLOCK_SIZE) {
        dbg_printf (DL_KDB, "[E]: The init size of char blocks is too big.\n");
    }

    for (int i = 0; i < block_size && block_size <= MEASURE_BLOCK_SIZE; ++i) {
        cblock[i] = 'A' + i;
    }
}

void dwt_stream_copy_access_unalignment (void)
{   
    uint64_t start  = 0;
    uint64_t end    = 0;
    uint64_t delta 	= 0;

	int max_offset = 512;
	int foo_offset = 0;
    
	for (int i = 0; i < max_offset; ++i) {
        start = *fetch_cyccnt ();
        
		for (int j = 1; j <= i; ++j) {
			foo_offset = j * sizeof (char);
 
            memcpy (cblock, cblock + foo_offset, foo_offset);
        }

        end = *fetch_cyccnt ();

        delta = (end - start); 
	
        dbg_printf (DL_KDB, "%ld, ", delta);
    }
}

void dwt_stream_copy_access_alignment (void)
{	
	uint64_t start = 0;
	uint64_t end = 0;
	uint64_t latency = 0;

	int base = 2;
	int n_iterations = base;
	int n_case = 8;

	dbg_printf (DL_KDB, "|%6s|%12s|\n", "Offset", "Clock Cycles");	

	for (int i = 1; i <= n_case; ++i) {
		n_iterations = base << i;

		start = *fetch_cyccnt ();

		for (int j = 1; j <= n_iterations; j += 4) {
			memcpy (cblock, cblock + n_iterations / 2 - 1, 4);
		}

		end = *fetch_cyccnt ();

		latency = (end - start);
		
        dbg_printf (DL_KDB, "|%6d|%12ld|\n", n_iterations, latency);
	}
}

void systicks_stream_copy_access_unalignment (void)
{   
    uint64_t start   = 0;
    uint64_t end     = 0;
    uint64_t latency = 0;

    int base         = 2;
    int n_iterations = base;
    int n_case       = 8;

	dbg_printf (DL_KDB, "|%6s|%8s|\n", "Offset", "Systicks");	
    
	for (int j = 0; j < n_case; ++j) {
        n_iterations = base << (j + 1); 

        start = *fetch_systicks ();
            
        for (int i = 1; i <= n_iterations; ++i) {
            memcpy (cblock, cblock + n_iterations / 2 - 1, i); 
        }   

        end = *fetch_systicks ();

        latency = fetch_systicks_consumption (start, end);
		
		dbg_printf (DL_KDB, "|%6d|%8ld|\n", n_iterations, latency);
    }   
}

void systicks_stream_copy_access_alignment (void)
{	
	uint64_t start = 0;
	uint64_t end = 0;
	uint64_t latency = 0;

	int base = 2;
	int n_iterations = base;
	int n_case = 8;

	dbg_printf (DL_KDB, "|%6s|%8s|\n", "Offset", "Systicks");	

	for (int i = 1; i <= n_case; ++i) {
		n_iterations = base << i;

		start = *fetch_systicks ();

		for (int j = 1; j <= n_iterations; j += 4) {
			memcpy (cblock, cblock + n_iterations / 2 - 1, 4);
		}

		end = *fetch_systicks ();

		latency = fetch_systicks_consumption (start, end);

		dbg_printf (DL_KDB, "|%6d|%8ld|\n", n_iterations, latency);
	}
}

void dwt_set_to_zero () {
	uint64_t start;
	uint64_t end;
	uint64_t latency;
	int n_case = 8;
	uint64_t base = 2;
	uint64_t set_blocks = base;

	dbg_printf (DL_KDB, "|%6s|%12s|\n", "Length", "Clock cycles");	
	
	for (int i = 0; i < n_case; ++i) {
		start = *fetch_cyccnt ();

		set_blocks = base << (i + 1);

		for (int j = 0; j < set_blocks; ++j) {
			memset (cblock, '0', j); 
		}

		end = *fetch_cyccnt ();

		latency = end - start;

		dbg_printf (DL_KDB, "|%6d|%12ld|\n", set_blocks, latency);
	}
}

void systicks_set_to_zero () {
	uint64_t start = 0;
	uint64_t end = 0;
	uint64_t latency = 0;
	int n_iterations = 8;
	int base = 2;
	int	set_blocks = base; 		

	dbg_printf (DL_KDB, "|%6s|%8s|\n", "Length", "Systicks");	
	
	for (int i = 0; i < n_iterations; ++i) {
		set_blocks = base << (i + 1);

		start = *fetch_systicks ();

		for (int j = 1; j <= set_blocks; ++j) {
			memset (cblock, '0', j);
		}
		
		end = *fetch_systicks ();

		latency = fetch_systicks_consumption (start, end);
	
		dbg_printf (DL_KDB, "|%6d|%8ld|\n", set_blocks, latency);
	}
}

void benchmark_main (void)
{
    init_char_block (MEASURE_BLOCK_SIZE);

	for (int i = 0; i < (sizeof (benchmark_functions) / sizeof (struct benchmark_t)); ++i) {
		dbg_printf (DL_KDB, "%s\n", benchmark_functions[i].name);
		dbg_printf (DL_KDB, "--------------------------------------------------\n");
		benchmark_functions[i].function ();
		dbg_printf (DL_KDB, "--------------------------------------------------\n");
	}
}

void benchmark_handler (void)
{
	benchmark_main ();
}
