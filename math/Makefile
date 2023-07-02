
CXX = ccache g++
CXX_FLAGS = -c -g -O2 -mavx2
CXX_INCLUDES = -I.
CXX_LINK = 


main.o: main.cpp
	$(CXX) $(CXX_FLAGS) $(CXX_INCLUDES) main.cpp -o main.o

main: main.o
	$(CXX) $(CXX_LINK) main.o -o main


.PHONY: all

all: main
	./main

bench: main
	bash -c 'time ./main'
