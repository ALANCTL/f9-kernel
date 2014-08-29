#include <stdio.h>
#include <stdint.h>
#include <assert.h>

#define INTERVAL 0xFFFF

void init_sequence (const int seed, uint64_t* const random_sequence);

uint64_t congruential_generator (uint64_t* const random_sequence);

int main (int argc, char* argv[], char* env[])
{
	const int seed = 13;
	uint64_t random_sequence[INTERVAL];

	init_sequence (seed, random_sequence);	
	congruential_generator (random_sequence);
		
	assert (random_sequence != NULL);
	assert ((random_sequence + INTERVAL - 1) != NULL);
	
	return 0;
}

void init_sequence (const int seed, uint64_t* const random_sequence)
{
	uint64_t* const begin	= random_sequence + 1;
	uint64_t* const end		= random_sequence + INTERVAL - 1;
	uint64_t* iterator		= random_sequence;

	*iterator = seed;

	for (iterator = begin; iterator != end; ++iterator) {
		*iterator = 0;
	}
}

uint64_t congruential_generator (uint64_t* const random_sequence)
{
	const int		multiplier	= 0x41A7;
	const int		increment	= 0x0;
	const uint32_t	modulus		= 0xCCCCCCF;
	uint64_t* const begin		= random_sequence;
	uint64_t* const end			= random_sequence + INTERVAL - 2;

	for (uint64_t* iterator = begin; iterator != end; ++iterator) {
		*(iterator + 1) = (multiplier * (*iterator) + increment) % modulus;
	}
}

