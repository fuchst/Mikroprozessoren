#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <float.h>
#include <math.h>
#include <emmintrin.h>

int stride = 4160;

typedef struct memBlock{
	char memArray[64];
}mBlock;

static 
double gettimediff(struct timespec end, struct timespec start) {
	double ret = end.tv_sec + (double)end.tv_nsec/1000000000.0 - start.tv_sec - (double)start.tv_nsec/1000000000.0;

	return ret;
}

static 
mBlock* initArray(int numBlocks) { 
	mBlock* ret = (mBlock*)malloc(numBlocks*sizeof(mBlock));
	int i = numBlocks;
	for(; i > 0;)
	{
		ret[--i].memArray[0] = (i+stride)%numBlocks;
	}

	return ret;
}

static 
void doMeasure(int N, mBlock* block){
	int acc = block[0].memArray[0];
	int count = 0;
	int i = 0;
	for(; i<1000000; i++){
			acc = block[acc].memArray[0];
			//if(acc == 0)
			//	count ++;
			//if(count > 1)
			//	break;
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
	int N = 101;
	int i = 0;
	for(; i<10; i++)
	{
			
		int res = gcdr(N, stride);
		if(res == 1)
		{
			printf("GGT of %d and %d = %d", N, stride, res);
			printf("ran into if");
			mBlock* block = initArray(N);
			clock_gettime(CLOCK_MONOTONIC_RAW, &start);
			doMeasure(N, block);
			clock_gettime(CLOCK_MONOTONIC_RAW, &end);
			double result = gettimediff(end, start) / (double)1000000;
			printf("Result with N = %d: %2.9f\n", N, result); 
			printf("%d\n", N);
		}
		N = N + 101;
	}


	return 0;
}
