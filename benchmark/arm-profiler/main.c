#include "main.h"

#define CORE_CLOCK						(0x0a037a00)
#define SYSTICK_MAXRELOAD				(0x00ffffff)
#define RESULT_BYTES					(0x8000)
#define CONFIG_KTIMER_HEARTBEAT			(0x1000)

#define SCS_BASE                        (uint32_t) (0xE000E000) 
#define SYSTICK_BASE                    (SCS_BASE + 0x0010)                                     
#define SYSTICK_CTL                     (volatile uint32_t *) (SYSTICK_BASE)                                                                
#define SYSTICK_RELOAD                  (volatile uint32_t *) (SYSTICK_BASE + 0x04)             
#define SYSTICK_VAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x08)             
#define SYSTICK_CAL                     (volatile uint32_t *) (SYSTICK_BASE + 0x0C)             
#define SYSTICK_HANDLER					0xE000ED08

#define PERIPH_BASE                     (uint32_t) (0x40000000) 
#define AHB1PERIPH_BASE                 (PERIPH_BASE + 0x00020000)
#define RCC_BASE                        (AHB1PERIPH_BASE + 0x3800) 
#define RCC_AHB1ENR                     (volatile uint32_t *) (RCC_BASE + 0x30)
#define RCC_AHB1ENR_CCMDATARAMEN        (uint32_t) (1 << 20)

__attribute ((section (".ccm"))) char src[RESULT_BYTES];
__attribute ((section (".ccm"))) char dest[RESULT_BYTES];

uint64_t result[14];

void RCC_Configuration(void)
{
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOA, ENABLE);
}

void GPIO_Configuration(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_USART1);   
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_USART1);
}

void USART1_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;

	USART_InitStructure.USART_BaudRate = 115200;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);
}

void RNG_Configuration (void)
{
	RCC_AHB2PeriphClockCmd (RCC_AHB2Periph_RNG, ENABLE);
	RNG_Cmd (ENABLE);
}

void CCM_Configuration (void)
{
	*RCC_AHB1ENR |= RCC_AHB1ENR_CCMDATARAMEN;
}

void SysTick_Configuration (void)
{
	SysTick_Config (CONFIG_KTIMER_HEARTBEAT);
	NVIC_SetPriority (SysTick_IRQn, 1);
	NVIC_EnableIRQ (SysTick_IRQn);
}

void USART1_puts(char* s)
{
	while(*s) {
		while(USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
		USART_SendData(USART1, *s);
		s++;
	}
}

uint32_t delta_ticks (uint32_t n, uint32_t m) 
{
	return (n >= m) ? (n - m) : (m - n);
}

//int memcpy_baseline(char *dest, const char *src, int len);

void init_block (char *blk)
{
	for (int i = 0; i < 0x8000; ++i) {
		blk[i] = 'A' ^ i;	
	}
}

#pragma GCC optimize ("O0")
int main(void)
{
	RCC_Configuration ();
	GPIO_Configuration ();
	USART1_Configuration ();
	CCM_Configuration ();
	RNG_Configuration ();
	SysTick_Configuration ();

	uint32_t start = 0;
	uint32_t end = 0;
	uint64_t offset = 4;
	int n_iteration = 1000;

	for (int i = 0; i < 14; ++i) {
		result[i] = 0;
	}

	init_block (src);
	init_block (dest);

	for (uint32_t j = 0; j < 14; ++j) {
		offset = 4 << j;

		for (int i = 0; i < n_iteration; ++i) {
			start = *SYSTICK_VAL;

			memcpy (src, dest, offset);

			end = *SYSTICK_VAL;

			result[j] += delta_ticks (start, end);
		}
	}

	uint32_t rnd[12];

	for (int i = 0; i < 12; ++i) {
		rnd[i] = RNG_GetRandomNumber() % 12 + 1;
	}

	USART1_puts ("Profiling Finish.\r\n");

	while(1); 
}

#ifdef  USE_FULL_ASSERT
void assert_failed(uint8_t* file, uint32_t line)
{ 
	while (1)
	{
	}
}
#endif


