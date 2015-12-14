#include <stdio.h>
#include <time.h>
#include <iostream>

using namespace std;

const int N = 4096*4096;
clockid_t clockId = CLOCK_MONOTONIC_RAW;

static inline double getMilliSeconds(void) {
	struct timespec time;
	clock_gettime(clockId, &time);
	return (time.tv_sec * 1000) + (time.tv_nsec / 1000000.0);
}

static inline double getMicroSeconds(void) {
	struct timespec time;
	clock_gettime(clockId, &time);
	return (time.tv_sec * 1000000) + (time.tv_nsec / 1000.0);
}

char* initMemory() {
	char* mem = (char*) malloc(N * sizeof(char));
	for(int i = 0; i < N; i++) {
		mem[i] = 1;
	}
	return mem;
}

inline unsigned int stridedSum(char* mem, int stride) {
	unsigned int sum = 0;
	for(int i = 0; i < N-stride; i += stride){
		sum += mem[i];
	}
	return sum;
}

void doMemAccessMeasurements(char *mem, int stride) {
	cout << "-----------------------------------------------------------------" << endl;
	cout << "Stride: " << stride << endl;
	double start = getMicroSeconds();
	int sum = stridedSum(mem, stride);
	double end = getMicroSeconds();
	double totalMics = end - start;
	cout << "Microseconds per 100 memory accesses: " << (100.0*totalMics)/sum << " ." << endl;
	cout << "-----------------------------------------------------------------" << endl;
}

int main(int argc, char* argv[])
{
	cout << "Execution started." << endl;
	cout << "Allocating " << N << " bytes of memory..." << endl;
	char* mem = initMemory();
	for(int stride = 1; stride <= 1024; stride*=2) {
		doMemAccessMeasurements(mem, stride);
	}
	return 0;
}
