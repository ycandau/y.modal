#include "random.h"

// ====  RANDOM_N_OF_M  ====
// Choose n elements of a numbered list
// Reorganizes the list by iterating forward and permutating once within the remaining sublist
// Expects an array of m integers

void random_n_of_m(t_int32 n, t_int32 m, t_int32 *index_arr)
{
	// Initialize the array
	for (t_int32 i = 0; i < m; i++) { index_arr[i] = i; }

	// Iterate through the n first elements of the array
	t_int32 j, tmp;
	for (t_int32 i = 0; i < n; i++) {

		// Permutate the ith index with any index from i to (m-1)
		j = rand() % (m - i);
		tmp = index_arr[i];
		index_arr[i] = index_arr[i + j];
		index_arr[i + j] = tmp;	}
}
