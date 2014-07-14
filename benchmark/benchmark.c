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

static uint64_t *DWT_CYCCNT     = (uint64_t *) CYCLE_COUNT_REGADDR; 
static uint64_t *DWT_CONTROL    = (uint64_t *) CONTROL_REGADDR; 
static uint64_t *SCB_DEMCR      = (uint64_t *) DEBUG_EXCEPTION_MONITOR_CONTROL_REGADDR;

static int cnt_enable = 0;  

static char cblock[MEASURE_BLOCK_SIZE];

struct benchmark_t benchmark_functions[] = {
	{
		.name = "DWT SCA unalignment: ==================================",
		.function = dwt_stream_copy_access_unalignment
	},

	{
		.name = "DWT SCA alignment: ====================================",
		.function = dwt_stream_copy_access_alignment
	},

	{	
		.name = "Systicks SCA unalignment: =============================", 
		.function = systicks_stream_copy_access_unalignment
	},

	{
		.name = "Systicks SCA alignment: ===============================",
		.function = systicks_stream_copy_access_alignment
	},

	{
		.name = "DWT memset",
		.function = dwt_set_to_zero
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

void measure_alignment_unit (void)
{   
    uint64_t start  = 0;
    uint64_t end    = 0;

    start = *fetch_cyccnt ();

    for (int i = 0; i < 4; ++i) {
    } 
    
    end = *fetch_cyccnt ();

    dbg_printf (DL_KDB, "The consumption of alignment unit: %ld\n", end - start);   
}

void measure_maximum_char_blocks (void)
{
    uint64_t start  = 0;
    uint64_t end    = 0;

    start = *fetch_cyccnt ();
    
    for (int i = 0; i < MAX_BSS_BYTES; ++i) {
    }   

    end = *fetch_cyccnt ();

    dbg_printf (DL_KDB, "The consumption of maximum of char blocks: %ld\n", end - start);
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
    uint64_t start   = 0;
    uint64_t end     = 0;
    uint64_t latency = 0;

    int base         = 2;
    int n_iterations = base;
    int n_case       = 8;

	dbg_printf (DL_KDB, "Latency: [");

    for (int j = 0; j < n_case; ++j) {
        n_iterations = base << (j + 1);

        start = *fetch_cyccnt ();
        
		for (int i = 1; i <= n_iterations; ++i) {
            memcpy (cblock, cblock + n_iterations / 2 - 1, i);
        }

        end = *fetch_cyccnt ();

        latency = (end - start); // /n_iterations;

        //dbg_printf (DL_KDB, "The block size is %d bytes, the latency is %ld \n", n_iterations, latency);
		
		if (j != n_case - 1) 
        	dbg_printf (DL_KDB, "%ld, ", latency);
		else	
			dbg_printf (DL_KDB, "%ld]\n", latency);
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

	dbg_printf (DL_KDB, "Latency: [");

	for (int i = 1; i <= n_case; ++i) {
		n_iterations = base << i;

		start = *fetch_cyccnt ();

		for (int j = 1; j <= n_iterations; j += 4) {
			memcpy (cblock, cblock + n_iterations / 2 - 1, 4);
		}

		end = *fetch_cyccnt ();

		latency = (end - start);

        //dbg_printf (DL_KDB, "The block size is %d bytes, the latency is %ld \n", n_iterations, latency);
		
		if (i != n_case - 1) 
        	dbg_printf (DL_KDB, "%ld, ", latency);
		else	
			dbg_printf (DL_KDB, "%ld]\n", latency);
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

	dbg_printf (DL_KDB, "Latency: [");

    for (int j = 0; j < n_case; ++j) {
        n_iterations = base << (j + 1); 

        start = *fetch_systicks ();
            
        for (int i = 1; i <= n_iterations; ++i) {
            memcpy (cblock, cblock + n_iterations / 2 - 1, i); 
        }   

        end = *fetch_systicks ();

        latency = fetch_systicks_consumption (start, end);

        //dbg_printf (DL_KDB, "The block size is %d bytes, the latency is %ld \n", n_iterations, latency);
		
		if (j != n_case - 1) 
        	dbg_printf (DL_KDB, "%ld, ", latency);
		else	
			dbg_printf (DL_KDB, "%ld]\n", latency);
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
	
	dbg_printf (DL_KDB, "Latency: [");

	for (int i = 1; i <= n_case; ++i) {
		n_iterations = base << i;

		start = *fetch_systicks ();

		for (int j = 1; j <= n_iterations; j += 4) {
			memcpy (cblock, cblock + n_iterations / 2 - 1, 4);
		}

		end = *fetch_systicks ();

		latency = fetch_systicks_consumption (start, end);

        //dbg_printf (DL_KDB, "The block size is %d bytes, the latency is %ld \n", n_iterations, latency);
		
		if (i != n_case - 1) 
        	dbg_printf (DL_KDB, "%ld, ", latency);
		else	
			dbg_printf (DL_KDB, "%ld]\n", latency);
	}
}

void dwt_set_to_zero () {
	uint64_t start;
	uint64_t end;
	uint64_t latency;

	start = *fetch_cyccnt ();

	for (int i = 0; i < MEASURE_BLOCK_SIZE; ++i) {
		memset (cblock, '0', i); 
	}

	end = *fetch_cyccnt ();

	latency = end - start;

	dbg_printf (DL_KDB, "Latency: %ld\n", latency);
}

void benchmark_main (void)
{
    //measure_alignment_unit ();
    //measure_maximum_char_blocks ();

    init_char_block (MEASURE_BLOCK_SIZE);

	for (int i = 0; i < (sizeof (benchmark_functions) / sizeof (struct benchmark_t)); ++i) {
		dbg_printf (DL_KDB, "%s\n", benchmark_functions[i].name);
		dbg_printf (DL_KDB, "--------------------------------------------------\n");
		benchmark_functions[i].function ();
		dbg_printf (DL_KDB, "--------------------------------------------------\n");
	}
}

void benchmark_init (void)
{
	dbg_puts ("----------------------------------\n");
	dbg_puts ("-------|---------------------B----\n");
	dbg_puts ("-------|-------|-------------E----\n");
	dbg_puts ("-------|-------|---|---------N----\n");
	dbg_puts ("---|---|-------|---|---------C----\n");
	dbg_puts ("---|---|---|---|---|---------H----\n");
	dbg_puts ("---|---|---|---|---|---|-----M----\n");
	dbg_puts ("---|---|---|---|---|---|-----A----\n");
	dbg_puts ("---|---|---|---|---|---|-----R----\n");
	dbg_puts ("---|---|---|---|---|---|-----K----\n");
	dbg_puts ("----------------------------------\n");
	dbg_puts ("\n");
}

void benchmark_handler (void)
{
	//benchmark_init (); 
	benchmark_main ();
}

