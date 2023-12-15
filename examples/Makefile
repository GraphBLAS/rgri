
CXX = g++-13

MDSPAN_DIR = /Users/bbrock/pkg/mdspan
HDF5_DIR = /opt/homebrew/Cellar/hdf5/1.14.3
TBB_DIR = /opt/homebrew/Cellar/tbb/2021.11.0

SOURCES += $(wildcard *.cpp)
TARGETS := $(patsubst %.cpp, %, $(SOURCES))

GRB_DIR=../include

# MDSPAN_FLAGS = -I$(MDSPAN_DIR)/include
# BINSPARSE_FLAGS = -I$(HDF5_DIR)/include -DBINSPARSE_IO
# NWGRAPH_FLAGS = -I$(HOME)/src/NWGraph/include -DUSE_NWGRAPH
# TBB_FLAGS= -I$(TBB_DIR)/include -L$(TBB_DIR)/lib

# HDF5_LIBS = -L$(HDF5_DIR)/lib -lhdf5_hl_cpp -lhdf5_cpp -lhdf5_hl -lhdf5  -ltbb

CXXFLAGS = -std=c++20 -O3 -I$(GRB_DIR) -lfmt $(MDSPAN_FLAGS) $(BINSPARSE_FLAGS) $(TBB_FLAGS) $(NWGRAPH_FLAGS)

LD_FLAGS = $(HDF5_LIBS) -Wl,-ld_classic

all: $(TARGETS)

%: %.cpp
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS)

clean:
	rm -fv $(TARGETS)
