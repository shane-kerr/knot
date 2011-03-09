#include <config.h>
#include <limits.h>
#include <stdint.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include "common.h"
#include "universal-system.h"

/*----------------------------------------------------------------------------*/

const uint MAX_UINT_EXP = 32;
const unsigned long MAX_UINT_MY = 4294967295;

/*----------------------------------------------------------------------------*/
/* Private functions                                                          */
/*----------------------------------------------------------------------------*/

static void us_generate_coefs(us_system_t *system, uint from, uint to)
{
	assert(system != NULL);

	for (uint i = from; i < to; ++i) {
		int used = 0;

		do {
			// generate random odd number
			system->coefs[i] = rand() % MAX_UINT_MY;
			if (system->coefs[i] % 2 == 0) {
				system->coefs[i] = (system->coefs[i] == 0)
				                    ? 1
				                    : system->coefs[i] - 1;
			}
			// check if this coeficient is already used
			uint j = from;
			while (used == 0 && j < i) {
				if (system->coefs[j++] == system->coefs[i]) {
					used = 1;
				}
			}
			// if already used, generate again
		} while (used != 0);
	}
}

/*----------------------------------------------------------------------------*/
/* Public functions                                                           */
/*----------------------------------------------------------------------------*/

void us_initialize(us_system_t *system)
{
	assert(system != NULL);
	assert(UINT_MAX == MAX_UINT_MY);
	srand(time(NULL));

	// Initialize both generations of functions by generating random odd
	// numbers
	us_generate_coefs(system, 0, US_FNC_COUNT * GEN_COUNT);
}

/*----------------------------------------------------------------------------*/
/*!
 * \note \a generation starts from 1
 */
int us_next(us_system_t *system, uint generation)
{
	assert(system != NULL);
	// generate new coeficients for the new generation
	us_generate_coefs(system, (generation - 1) * US_FNC_COUNT,
	                  generation * US_FNC_COUNT);
	return 0;
}

/*----------------------------------------------------------------------------*/

uint32_t us_hash(const us_system_t *system, uint32_t value, uint table_exp,
                 uint fnc, uint generation)
{
	/*
	 * multiplication should overflow if larger than MAX_UINT
	 * this is the same as (coef * value) mod MAX_UINT
	 *
	 * TODO: maybe we should not rely on this
	 */
	assert(system != NULL);
	assert(table_exp <= 32);
	assert(fnc < US_FNC_COUNT);
	assert(generation <= GEN_COUNT);

	return ((system->coefs[((generation - 1) * US_FNC_COUNT) + fnc] * value)
	        >> (MAX_UINT_EXP - table_exp));
}
