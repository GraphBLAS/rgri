
DPCXX = dpcpp
HSCXX = syclcc

SOURCES += $(wildcard *.cpp)
DPCXX_TARGETS := $(patsubst %.cpp, %-dpcpp, $(SOURCES))
HSCXX_TARGETS := $(patsubst %.cpp, %-hsc, $(SOURCES))
TARGETS := $(DPCXX_TARGETS) $(HSCXX_TARGETS)

GRB_DIR=../../include

CXXFLAGS = -std=c++20 -I$(GRB_DIR)

# LDLIBS = -lfmt

# HIPSYCL_FLAGS = --hipsycl-targets='cuda:sm_75'

all: $(TARGETS)

run: all
	@for target in $(foreach target,$(TARGETS),./$(target)) ; do echo "Running \"$$target\"" ; $$target ; done


dpcpp: $(DPCXX_TARGETS)

hipsycl: $(HSCXX_TARGETS)

%-dpcpp: %.cpp
	$(DPCXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS) $(DPCPP_FLAGS) $(LDLIBS)

%-hsc: %.cpp
	$(HSCXX) $(CXXFLAGS) -o $@ $^ $(LD_FLAGS) $(HIPSYCL_FLAGS) $(LDLIBS)

clean:
	rm -fv $(TARGETS)
