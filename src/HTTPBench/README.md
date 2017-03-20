The project is for simple benchmarking of HTTP GET requests on a webserver.

The project depends on libcurl (included with the source). To build libcurl, cd into the HTTPBench/curl directory and then run:

1. `./configure --prefix=$(pwd)../`
2. `make` to build the library
3. `make test` if you want to run the tests
4. `make install` to install the library

To build HTTPBench cd into HTTPBench/ and then run:
```
make
```

To run HTTPBench cd into HTTPBench/ and then run:
```
./httpbench myurltotest.com:myport number_of_iterations number_of_threads
```
