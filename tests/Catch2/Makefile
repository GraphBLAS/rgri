GRB_DIR=../include

all: tests

run: all
	./tests

test: all
	./tests

tests: tests.cpp
	g++-13 tests.cpp -o tests -std=c++20 -O3 -I$(GRB_DIR)

clean:
	rm -fv tests
