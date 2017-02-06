The project is for simple benchmarking of HTTP GET requests on a webserver.

The project depends on libcurl (included with the source). To build libcurl, cd into the HTTPBench/curl directory and then run:

1. `make` to build the library
2. `make test` if you want to run the tests
3. `sudo make install` to install the library
4. `sudo ldconfig` to update the linker so it can find the newly built library


To build HTTPBench cd into HTTPBench/ and then run:
```
make
```

To run HTTPBench cd into HTTPBench/ and then run:
```
./httpbench myurltotest.com:myport number_of_http_get_iterations
```
