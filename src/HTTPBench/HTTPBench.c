#include <stdio.h>
#include <stdlib.h> // exit
#include <sys/time.h> // gettimeofday

#include <curl/curl.h>


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
long long elapsed_ms(struct timeval start, struct timeval finish)
{
	long long elapsedms;

	elapsedms = (finish.tv_sec - start.tv_sec) * 1000;
	elapsedms += (finish.tv_usec - start.tv_usec) / 1000;
	return elapsedms;
}

int main(int argc, char **args)
{
	/*curl*/
 	CURL *curl;
 	CURLcode res;
 	const char *target_url;

	/*timing*/
	struct timeval start_time, end_time;
	long long duration_ms;

	/*other*/
	char *endptr;
	unsigned int iterations;
	

 	if (argc != 3) usage();
 	target_url = args[1];
	iterations = strtol(args[2], &endptr, /*base 10*/ 10);
	if (iterations == 0){
		printf("Unable to parse iterations\n");
		usage();
	}

	curl = curl_easy_init();
	if(curl) {
		gettimeofday(&start_time, NULL);
		for (int i = 0; i < iterations; i++) {
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
		gettimeofday(&end_time, NULL);
	} else {
		printf("Error initializing curl: curl_easy_init");
	}
	curl_easy_cleanup(curl);
	
	duration_ms = elapsed_ms(start_time, end_time); 
	printf("Total Duration: %lld ms\n", duration_ms);
	printf("Average Time Per HTTP GET: %.2f ms\n", (double) duration_ms / iterations);
	return 0;
}
