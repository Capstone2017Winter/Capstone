/* 
 * A light-weight benchmarking tool
 * used to load-test and stress-test
 * small webservers. Based upon libcurl
 * and pthreads.
 * Written by: Elias Carter
 */

#include <stdio.h>
#include <stdlib.h> // exit
#include <sys/time.h> // gettimeofday
#include <pthread.h>

#include <curl/curl.h>

/*
 * Global read-only variable which contains target url
 * that we are benchmarking. It is assigned to when 
 * program arguments are parsed, and then is only read from
 * thereafter
 */
const char *target_url;

static void usage()
{
	fprintf(stderr,
"Usage:\n\
httpbench [url] [iterations] [threads]\n");
	exit(EXIT_FAILURE);
}

/* 
 * A dummy callback which is called after a GET request is successful
 */
static size_t dummy_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	/* dont do anything with recieved data just return 'read' size*/
	return size * nmemb;
}

/*
 * Get process time in ms
 */
static long long elapsed_ms(struct timeval start, struct timeval finish)
{
	long long elapsedms;

	elapsedms = (finish.tv_sec - start.tv_sec) * 1000;
	elapsedms += (finish.tv_usec - start.tv_usec) / 1000;
	return elapsedms;
}

/*
 * Main routine run on benchmarking threads.
 */
static void* benchmark_routine(void * iter)
{
	/*curl*/
	CURL *curl;
 	CURLcode res;

	/*timing*/
	struct timeval start_time, end_time;
	long long iterations = (long long)iter;
	

	curl = curl_easy_init();
	if(curl) {
		gettimeofday(&start_time, NULL);
		for (int i = 0; i < iterations; i++) {
			curl_easy_setopt(curl, CURLOPT_URL, /*read-only global*/ target_url);

			/* redirect if needed*/ 
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			/* set handler for writing recieved data*/
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dummy_write);

			/* do request */ 
			res = curl_easy_perform(curl);

			/* Check for errors */ 
			if(res != CURLE_OK) {
				fprintf(stderr, 
					"curl_easy_perform() failed: %s\n", 
					curl_easy_strerror(res));
			} 
		}
		gettimeofday(&end_time, NULL);
	} else {
		fprintf(stderr, "Error initializing curl: curl_easy_init");
	}
	curl_easy_cleanup(curl);
	
	/* 
	 * void* must be the same size as long long, or else we are going 
	 * to have problems. For 64 bit machines, we should be alright
	 */
	return (void*)elapsed_ms(start_time, end_time); 
} 

int main(int argc, char **args)
{

	/*other*/
	long long iterations;
	int i;

	/*threading*/
	pthread_t *tids;
	long long *benchmark_times; /* store execution times for each threaded benchmark */
	unsigned int thread_count;
	int error;

	/*
	 * We can only run if sizeof(void*)==sizeof(long long)
	 * due to the way we are passing parameters through 
	 * pthread_join. For 64-bit machines we should be alright.
	 */
	if (sizeof(void*) != sizeof(long long)) {
		fprintf(stderr,"The implementation of this program requires \
			sizeof(void*) == sizeof(long long). \
			Try running on a 64 bit machine.\n");
		exit(EXIT_FAILURE);
	}
	
	/* parse arguments */
 	if (argc != 4) usage();
 	target_url = args[1];
	iterations = strtol(args[2], NULL, /*base 10*/ 10);
	if (iterations == 0) {
		fprintf(stderr, "Unable to parse iterations\n");
		usage();
	}
	thread_count = strtol(args[3], NULL, /*base 10*/ 10);
	if (thread_count == 0) {
		fprintf(stderr, "Unable to parse thread count\n");
		usage();
	}

	/*
	 * Not thread safe, we must initialize curl library before
	 * we start creating threads.
	 */
	if(curl_global_init(CURL_GLOBAL_ALL) != 0) {
		fprintf(stderr, "curl_global_init failed\n");
		exit(EXIT_FAILURE);
	}

	tids = malloc(sizeof(pthread_t) * thread_count);
	benchmark_times = malloc(sizeof(long long) * thread_count);

	if (tids == NULL || benchmark_times == NULL) {
		fprintf(stderr, "malloc failed\n");
		exit(EXIT_FAILURE);
	}

	for(i=0; i<thread_count; i++) {
		error = pthread_create(&tids[i],
					NULL, /* default thread attributes */
					benchmark_routine,
					(void*)iterations);
		if (0 != error)
			fprintf(stderr, 
				"Couldn't start thread number %d, errno %d\n", 
				i, 
				error);
	}

	for(i=0; i<thread_count; i++){
		/* 
		 * sizeof(void*) == sizeof(long long) on 64 bit machines
		 * otherwise, we are going to have a problem with truncation
		 */
		error = pthread_join(tids[i], (void*)&benchmark_times[i]);
	}

	for(i=0; i<thread_count; i++){
		printf("[Thread %d] Average Time Per HTTP GET: %.2f ms\n", 
			i, 
			(double) benchmark_times[i] / iterations); 
	}

	free(tids);
	free(benchmark_times);
	return 0;
}

/* 
 * A light-weight benchmarking tool
 * used to load-test and stress-test
 * small webservers. Based upon libcurl
 * and pthreads.
 * Written by: Elias Carter
 */
