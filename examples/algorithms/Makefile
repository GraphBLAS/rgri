
CXX = g++

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

GRB_DIR=../../include

CXXFLAGS = -std=c++20 -I$(GRB_DIR) -lfmt

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -fv $(TARGETS)
