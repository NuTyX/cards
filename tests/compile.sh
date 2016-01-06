g++ -DNDEBUG -std=c++11 -O2 -Werror -pedantic -D_LARGEFILE_SOURCE -D_FILE_OFFSET_BITS=64 -fPIC   -c -o test.o test.cpp
g++  test.o  -o test -lcards -lcurl -larchive
[ -f test.o ]  && rm test.o
