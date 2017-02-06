#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <limits.h> // UINT_MAX
#include <time.h> // clock

void usage()
{
	printf(
"Usage:\n\
httpbench [url] [iterations]\n");
	exit(EXIT_FAILURE);
}

size_t dummy_write(char *ptr, size_t size, size_t nmemb, void *userdata)
{
	/* dont do anything with recieved data just return 'read' size*/
	return size * nmemb;
}

/*get process time in ms*/
long long clock_ms()
{
	clock_t clock_time;
	int CLOCKS_PER_MS; 
	
	CLOCKS_PER_MS = CLOCKS_PER_SEC / 1000;
	clock_time = clock();
	return clock_time / CLOCKS_PER_MS;
}

int main(int argc, char **args)
{
 	CURL *curl;
 	CURLcode res;
 	const char *target_url;
	char *endptr;
	unsigned int iterations;
	long long start_time, end_time;

 	if (argc != 3) usage();
 	target_url = args[1];
	iterations = strtol(args[2], &endptr, /*base 10*/ 10);
	if (iterations == 0){
		printf("Unable to parse iterations\n");
		usage();
	}

	curl = curl_easy_init();
	if(curl) {
		start_time = clock_ms();
		while(iterations--) {
			curl_easy_setopt(curl, CURLOPT_URL, target_url);

			/* redirect if needed*/ 
			curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);

			/* set handler for writing recieved data*/
			curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, &dummy_write);

			/* do request */ 
			res = curl_easy_perform(curl);
			/* Check for errors */ 
			if(res != CURLE_OK) {
				fprintf(stderr, "curl_easy_perform() failed: %s\n", curl_easy_strerror(res));
			} 
		}
		end_time = clock_ms();
	} else {
		printf("Error initializing curl: curl_easy_init");
	}
	curl_easy_cleanup(curl);
	
	printf("Total Duration: %lld ms\n", end_time - start_time);
	return 0;
}
