#!/bin/bash

echo "Doing 5 requests on single process server"
echo; time ./wclient 127.0.0.1 8080 "/spin.cgi?1" "/spin.cgi?1" "/spin.cgi?1" "/spin.cgi?1" "/spin.cgi?1"
echo; echo "Same requests on concurrent server with 5 threads"
time ./wclient 127.0.0.1 8081 "/spin.cgi?1" "/spin.cgi?1" "/spin.cgi?1" "/spin.cgi?1" "/spin.cgi?1"

