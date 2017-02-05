#include <stdio.h>
#include <curl/curl.h>
#include <stdlib.h>
#include <limits.h> // UINT_MAX

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

int main(int argc, char **args)
{
 	CURL *curl;
 	CURLcode res;
 	const char *target_url;
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
	}
	curl_easy_cleanup(curl);
	return 0;
}
