GRB_DIR=../include

all: basic_matrix

basic_matrix: basic_matrix.cpp
	g++ basic_matrix.cpp -o basic_matrix -std=gnu++17 -O3 -I$(GRB_DIR)

clean:
	rm -fv basic_matrix
