#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <emmintrin.h>
#include <stdint.h>

int stride = 4160;

typedef struct memBlock{
	uint64_t addr; //8byte
	char memArray[56];
}mBlock;

static 
double gettimediff(struct timespec end, struct timespec start) {
	double ret = end.tv_sec + (double)end.tv_nsec/1000000000.0 - start.tv_sec - (double)start.tv_nsec/1000000000.0;

	return ret;
}

static 
mBlock* initArray(int numBlocks) {
	//printf("size of one block: %d", sizeof(mBlock));
	mBlock* ret = (mBlock*)malloc(numBlocks*sizeof(mBlock));
	int i = numBlocks;
	for(; i > 0;)
	{
		ret[--i].addr = (i+stride)%numBlocks;
	}

	return ret;
}

static 
void doMeasure(int N, mBlock* block){
	int acc = block[0].addr;
	int i = 0;
	for(; i<1000000; i++){
			acc = block[acc].addr;
	}
}

static void doNAccess(int N, mBlock* block){
	int acc = block[0].addr;
	int i = 0;
	for(; i<N; i++){
			acc = block[acc].addr;
	}
}

int gcdr ( int a, int b )
{
  if ( a==0 ) return b;
  return gcdr ( b%a, a );
}

int main(int argc, char* argv[]) 
{
	struct timespec start;
	struct timespec end;
	int N = 1;
	int i = 0;
	for(; i<10000; i++)
	{
			
		int res = gcdr(N, stride);
		if(res == 1)
		{
			//printf("GGT of %d and %d = %d", N, stride, res);
			//printf("ran into if");
			mBlock* block = initArray(N);
			//do N accesses beforehand
			doNAccess(N, block);
			clock_gettime(CLOCK_MONOTONIC_RAW, &start);
			doMeasure(N, block);
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			double result = gettimediff(end, start) / (double)1000000;
			printf("Result with %d Byte: %2.9f\n", N*64, result); 
			printf("%d\n", N);
			free(block);
		}
		N = N + 10;
	}


	return 0;
}
