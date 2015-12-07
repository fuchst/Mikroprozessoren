#include <stdio.h>
#include <sys/time.h>

static inline
double gettime(void) {
	gettimeofday(&time, NULL);
	return time.tv_sec + time.tv_usec / 1000000.0;
}

int main(int argc, char* argv[])
{
	// TODO: Implementation
	
	return 0;
}
