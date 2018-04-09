#ifndef YC_RANDOM_H_
#define YC_RANDOM_H_

// ========  HEADER FILE FOR MISCELLANEOUS RANDOM FUNCTIONS  ========

#include "stdlib.h"					// For RAND_MAX
#include "max_types.h"			// For t_int32
#include "z_sampletype.h"		// For t_double

// ========  INLINE FUNCTION DEFINITIONS  ========

// ====  RANDOM_INT  ====
// Choose a random int between min and max

__inline t_int32 random_int(t_int32 min, t_int32 max)
{
	return ((min == max) ? min : (t_int32)(min + (max - min) * ((t_double)rand() / RAND_MAX)));
}

// ====  RANDOM_FLOAT  ====
// Choose a random float between min and max

__inline t_double random_float(t_double min, t_double max)
{
	return ((min == max) ? min : (min + (max - min) * rand() / RAND_MAX));
}

// ====  RANDOM_TIME_TO_SMP  ====
// Choose a random number of samples corresponding to a time between min and max

__inline t_int32 random_time_to_smp(t_double min, t_double max, t_double msr)
{
	return ((min == max) ? (t_int32)(min * msr) : (t_int32)((min + (max - min) * rand() / RAND_MAX) * msr));
}

// ========  FUNCTION DECLARATIONS  ========

void random_n_of_m(t_int32 n, t_int32 m, t_int32 *index_arr);

// ========  END OF HEADER FILE  ========

#endif