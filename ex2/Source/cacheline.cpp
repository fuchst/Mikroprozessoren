#include <stdio.h>
#include <sys/time.h>
#include <iostream>

using namespace std;

const int N = 1024*4096;
const int stride = 1;

static inline double getMilliSeconds(void) {
	struct timeval time;
	gettimeofday(&time, NULL);
	return (time.tv_sec * 1000) + (time.tv_usec / 1000.0);
}

char* initMemory() {
	return (char*) malloc(N * sizeof(char));
}

inline unsigned int stridedSum(char* mem, int stride) {
	unsigned int sum = 0;
	for(int i = 0; i < N-stride; i += stride){
		sum += mem[i];
	}
}

void doMemAccessMeasuremnts(char* mem, int stride) {
	cout << "Starting memory access performance measurement..." << endl;
	cout << "Stride: " << stride << endl;
	double start = getMilliSeconds();
	stridedSum(mem, stride);
	double end = getMilliSeconds();
	double totalMillis = end - start;
	cout << "Total time of execution: " << totalMillis << " milliseconds." << endl;
}

int main(int argc, char* argv[])
{
	cout << "Execution started." << endl;
	cout << "Allocating " << N << " bytes of memory..." << endl;
	char* mem = initMemory();
	doMemAccessMeasuremnts(mem, stride);
	return 0;
}
