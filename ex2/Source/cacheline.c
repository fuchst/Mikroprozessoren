#include <stdio.h>
#include <stdlib.h>
#include <time.h>

const int n = 4096 * 1024;
const int stride = 1024;

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

static inline
double gettimediff(struct timespec end, struct timespec start) {
	double ret = end.tv_sec + (double)end.tv_nsec/1000000000.0 - start.tv_sec - (double)start.tv_nsec/1000000000.0;

	return ret;
}


int main(int argc, char* argv[]) 
{
	int* array = (int*)ivalloc(1, n, sizeof(int));

	int dummy = 0;

	struct timespec start;
	struct timespec end;

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);

	for(int i = n-1; i >= 0; i -= stride) 
	{
		dummy += array[i];
	} 

	clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);

	double t = gettimediff(end, start);

	printf("Values read: %d\n", dummy);

	printf("Time per element: %2.9f\n", t / dummy);

	printf("Total time: %2.9f\n", t); 

	free(array);
	
	return 0;
}
