#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <nmmintrin.h>
#include <immintrin.h>
#include "options.h"

int debug = 0;
double *results;
double *ratios;
unsigned long   *sizes;
struct timeval time;

int no_sz = 1, no_ratio =1, no_version=1;

static inline
double gettime(void) {
	gettimeofday(&time, NULL);
	return time.tv_sec + time.tv_usec / 1000000.0;
}

static int checkTypeCase(char * text) {
	int count = 0;

	for(int i = 0; text[i] != '\0'; i++) {
		if(text[i] > 0x5A) count += 1;	
	}

	return count;
}

static void toupper_simple(char * text) {
	for(int i = 0; text[i] != '\0'; i++) {
		if(text[i] > 0x5A) text[i] -= 0x20;
	}
}

static void toupper_sse(char * text) {
	__m128i simddataOld;
	__m128i simddataNew;
	__m128i comparator;
	__m128i compresult;
	__m128i subtractor;

	comparator = _mm_set1_epi8(0x5A);
	subtractor = _mm_set1_epi8(0x20);

	unsigned int textlen = strlen(text);

	unsigned int iterations = textlen / 16;

	//#pragma omp parallel for schedule(static, iterations/2) 
	for(int i = 0; i < iterations; i++)
	{
		void * address = (void*)text+(i*16);
		simddataOld = _mm_load_si128(address);
		compresult = _mm_cmpgt_epi8(simddataOld, comparator);
		simddataNew = _mm_sub_epi8(simddataOld, subtractor);
		simddataNew = _mm_blendv_epi8(simddataOld, simddataNew, compresult);
		_mm_store_si128(address, simddataNew);
	}

	toupper_simple((void*)text+iterations*16);
}

static void toupper_avx(char * text) {
	__m256i simddataOld;
	__m256i simddataNew;
	__m256i comparator;
	__m256i compresult;
	__m256i subtractor;

	comparator = _mm256_set1_epi8(0x5A);
	subtractor = _mm256_set1_epi8(0x20);

	unsigned int textlen = strlen(text);

	unsigned int iterations = textlen / 32;

	//#pragma omp parallel for schedule(static, iterations/2) 
	for(int i = 0; i < iterations; i++)
	{
		void * address = (void*)text+(i*32);
		simddataOld = _mm256_load_si256(address);
		compresult = _mm256_cmpgt_epi8(simddataOld, comparator);
		simddataNew = _mm256_sub_epi8(simddataOld, subtractor);
		simddataNew = _mm256_blendv_epi8(simddataOld, simddataNew, compresult);
		_mm256_store_si256(address, simddataNew);
	}

	toupper_simple((void*)text+iterations*32);
}

static void toupper_asm(char * text) {
	__asm__ __volatile__ (
			"movq $0, %%rsi\n\t"                // init rsi with 0 (offset of address)
			"loop:\n\t"                         
			"movb 0(%%rbx, %%rsi, 1), %%al\n\t" // load value at rbx + (rsi * 1) - 0
			"cmp $0, %%al\n\t"                  // check if string end is reached
			"je end\n\t"                            
			"inc %%rsi\n\t"                     // increment offset
			"cmp $0x5A, %%al\n\t"               // compare letter case
			"jl loop\n\t"                       
			"sub $0x20, %%al\n\t"               // make large case
			"movb %%al, -1(%%rbx, %%rsi, 1)\n\t"// store value
			"jmp loop\n\t"
			"end:\n\t"
			:               /* no output registers  */
			: "b" (text)    /* input registers      */
			: "al", "rsi"   /* clobbered registers  */
			);
}


/*****************************************************************/


// align at 16byte boundaries
void* mymalloc(unsigned long int size)
{
	void* addr = malloc(size+32);
	return (void*)((unsigned long int)addr /16*16+16);
}

char createChar(int ratio){
	char isLower = rand()%100;

	// upper case=0, lower case=1
	if(isLower < ratio)
		isLower =0;
	else
		isLower = 1;

	char letter = rand()%26+1; // a,A=1; b,B=2; ...

	return 0x40 + isLower*0x20 + letter;

}

char * init(unsigned long int sz, int ratio){
	int i=0;
	char *text = (char *) mymalloc(sz+1);
	srand(1);// ensures that all strings are identical
	for(i=0;i<sz;i++){
		char c = createChar(ratio);
		text[i]=c;
	}
	text[i] = '\0';
	return text;
}



/*
 * ******************* Run the different versions **************
 */

typedef void (*toupperfunc)(char *text);

void run_toupper(int size, int ratio, int version, toupperfunc f, const char* name)
{
	double start, stop;
	int index;

	index =  ratio;
	index += size*no_ratio;
	index += version*no_sz*no_ratio;

	char *text = init(sizes[size], ratios[ratio]);


	if(debug) printf("Before: %.40s...\n",text);

	start = gettime();
	(*f)(text);
	stop = gettime();
	results[index] = stop-start;

	if(debug) printf("After:  %.40s...\n",text);
}

struct _toupperversion {
	const char* name;
	toupperfunc func;
} toupperversion[] = {
	{ "simple",    toupper_simple },
	{ "sse", toupper_sse },
	{ "avx", toupper_avx },
	{ "asm", toupper_asm },
	{ 0, 0 }
};


void run(int size, int ratio)
{
	int v;
	for(v=0; toupperversion[v].func !=0; v++) {
		run_toupper(size, ratio, v, toupperversion[v].func, toupperversion[v].name);
	}

}

void printresults(){
	int i,j,k,index;
	printf("%s\n", OPTS);

	for(j=0;j<no_sz;j++){
		for(k=0;k<no_ratio;k++){
			printf("Size: %ld \tRatio: %f \tRunning time:", sizes[j], ratios[k]);
			for(i=0;i<no_version;i++){
				index =  k;
				index += j*no_ratio;
				index += i*no_sz*no_ratio;
				printf("\t%s: %f", toupperversion[i].name, results[index]);
			}
			printf("\n");
		}
	}
}

int main(int argc, char* argv[])
{
	unsigned long int min_sz=800000, max_sz = 0, step_sz = 10000;
	int min_ratio=50, max_ratio = 0, step_ratio = 1;
	int arg,i,j,v;
	int no_exp;

	for(arg = 1;arg<argc;arg++){
		if(0==strcmp("-d",argv[arg])){
			debug = 1;
		}
		if(0==strcmp("-l",argv[arg])){
			min_sz = atoi(argv[arg+1]);
			if(arg+2>=argc) break;
			if(0==strcmp("-r",argv[arg+2])) break;
			if(0==strcmp("-d",argv[arg+2])) break;
			max_sz = atoi(argv[arg+2]);
			step_sz = atoi(argv[arg+3]);
		}
		if(0==strcmp("-r",argv[arg])){
			min_ratio = atoi(argv[arg+1]);
			if(arg+2>=argc) break;
			if(0==strcmp("-l",argv[arg+2])) break;
			if(0==strcmp("-d",argv[arg+2])) break;
			max_ratio = atoi(argv[arg+2]);
			step_ratio = atoi(argv[arg+3]);
		}

	}
	for(v=0; toupperversion[v].func !=0; v++)
		no_version=v+1;
	if(0==max_sz)  no_sz =1;
	else no_sz = (max_sz-min_sz)/step_sz+1;
	if(0==max_ratio)  no_ratio =1;
	else no_ratio = (max_ratio-min_ratio)/step_ratio+1;
	no_exp = v*no_sz*no_ratio;
	results = (double *)malloc(sizeof(double[no_exp]));
	ratios = (double *)malloc(sizeof(double[no_ratio]));
	sizes = (long *)malloc(sizeof(long[no_sz]));

	for(i=0;i<no_sz;i++)
		sizes[i] = min_sz + i*step_sz;
	for(i=0;i<no_ratio;i++)
		ratios[i] = min_ratio + i*step_ratio;

	for(i=0;i<no_sz;i++)
		for(j=0;j<no_ratio;j++)
			run(i,j);

	printresults();
	return 0;
}
