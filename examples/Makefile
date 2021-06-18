
CXX = g++

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

GRB_DIR=../include

CXXFLAGS = -std=gnu++17 -O3 -I$(GRB_DIR)

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -fv $(TARGETS)
