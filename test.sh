#!/bin/bash
c++ -std=c++14 -Wall ./test/token_test.cc && ./a.out
c++ -std=c++14 -Wall ./test/lexer_test.cc && ./a.out
