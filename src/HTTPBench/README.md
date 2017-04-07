The project is for simple benchmarking of HTTP GET requests on a webserver.

The project depends on libcurl (included with the source). To build libcurl, cd into the HTTPBench/curl-7.52.1 directory and then run:

1. `./configure --prefix=$(pwd)../`
2. `make` to build the library
3. `make test` if you want to run the tests
4. `make install` to install the library

After the library has been compiled it will be output to the HTTPBench/curl-7.52.1..
The library has been pre-compiled for a linux x86_64 system for convenience. You can compile for another architechture by deleting the pre-compiled library and run steps 1-4 above.

To build HTTPBench cd into HTTPBench/ and then run:
```
make
```

To run HTTPBench cd into HTTPBench/ and then run:
```
./httpbench myurltotest.com:myport number_of_iterations number_of_threads
```

