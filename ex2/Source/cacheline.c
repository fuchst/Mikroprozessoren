#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <emmintrin.h>

const int n = 4096 * 1024;

// New stride calculated by stride = stride * strideScale
int stride = 1;
const int strideScale = 2;

// Number of test iterations for each stride
const int iterations = 10;

//  Allocate memory of size size
static 
int* ivalloc(int value, size_t num, size_t size) { 
	int* ret = (int*)malloc(num*size);
	
	for(int i = num; i > 0;)
	{
		ret[--i] = value;
	}

	return ret;
}

static 
double gettimediff(struct timespec end, struct timespec start) {
	double ret = end.tv_sec + (double)end.tv_nsec/1000000000.0 - start.tv_sec - (double)start.tv_nsec/1000000000.0;

	return ret;
}

static 
double findMinimum(double* values, size_t size) {	
	double ret = DBL_MAX;

	for(int i = 0; i < size; i++) 
	{
		ret = (ret > values[i]) ? values[i] : ret;	
	}

	return ret;
}

static 
void clearCacheRange(int* start, int* end) {
	while(start <= end)
	{
		_mm_clflush(start++);
	}

	_mm_sfence();
}

struct result {
	int stride;
	double time;
};

int main(int argc, char* argv[]) 
{
	int* array = (int*)ivalloc(1, n, sizeof(int));

	int cycles = (int)floor(log((double)n/(double)stride)/log((double)strideScale));

	struct timespec start;
	struct timespec end;

	// Store values of iterations to later determine minimum
        double iterationResults[iterations];

	// Store final results
	struct result results[cycles];

	for(int k = 0; k < cycles; k++)
	{
		for(int i = 0; i < iterations; i++)
		{
			// Flush data cache
			clearCacheRange(&array[0], &array[n-1]);

			int count = 0;

			clock_gettime(CLOCK_MONOTONIC_RAW, &start);

			for(int j = 0; j < n; j += stride) 
			{
				count += array[j];
			} 

			clock_gettime(CLOCK_MONOTONIC_RAW, &end);

			iterationResults[i] = gettimediff(end, start) / (double)count;
		}

		struct result temp;
		temp.stride = stride;
		temp.time = findMinimum(iterationResults, iterations);

		results[k] = temp;

		stride *= strideScale;
	}

	for(int i = 0; i < cycles; i++) {
		printf("Stride: %d Time: %2.9f\n", results[i].stride, results[i].time);
	}

	free(array);
	
	return 0;
}
