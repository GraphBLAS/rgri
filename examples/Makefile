GRB_DIR=../include

all: basic_matrix vector

basic_matrix: basic_matrix.cpp
	g++ basic_matrix.cpp -o basic_matrix -std=gnu++17 -O3 -I$(GRB_DIR)

vector: vector.cpp
	g++ vector.cpp -o vector -std=gnu++17 -O3 -I$(GRB_DIR)

test: test.cpp
	g++ test.cpp -o test -std=gnu++17 -O3 -I$(GRB_DIR)

test_matrix: test_matrix.cpp
	g++ test_matrix.cpp -o test_matrix -std=gnu++17 -O3 -I$(GRB_DIR)

clean:
	rm -fv basic_matrix vector test test_matrix
