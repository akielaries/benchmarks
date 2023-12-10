# USE G++-10 for baremetal testing, G++-12 for Docker use
# Check if running within a Docker container
IS_DOCKER := $(shell test -f /.dockerenv && echo 1)

ifeq ($(IS_DOCKER),1)
    # If running within a Docker container, use g++-12
    CC := g++-12
else
    # If not running within a Docker container, use g++-10
    CC := g++-10
endif

NVCC 	= nvcc

PROJDIR = $(realpath $(CURDIR))
SRCDIR	= $(PROJDIR)/src
CPP		= $(shell find $(PROJDIR)/src -name '*.cpp')
SRC 	= benchmark.cpp $(CPP) -lpthread
BIN 	= benchsys
OBJDIR  = $(PROJDIR)/obj

# Check if nvcc (CUDA compiler) is available
ifeq ($(shell command -v nvcc -V 2> /dev/null),)
	CUDA_SRC 	=
	HAS_NVCC 	=
	STD			= -std=c++20 -march=native
	FLGS		= -pg -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare
else
	CC 			= nvcc
	CUDA    	= $(shell find $(PROJDIR)/src -name '*.cu')
	CUDA_SRC 	= $(CUDA)
	HAS_NVCC 	= -D__HAS_NVCC__
	STD			=
	SRC			+= $(CUDA)
	FLGS		= -pg -g -Wno-deprecated-gpu-targets
	OBJ_CUDA	= $(patsubst $(SRCDIR)/%.cu,$(OBJDIR)/%.o,$(CUDA))

$(OBJDIR)/%.o: $(SRCDIR)/%.cu
	@mkdir -p $(@D)
	$(NVCC) -c ${HAS_NVCC} $(FLGS) $< -o $@

endif
OBJ_CPP = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(CPP))
OBJ = $(patsubst $(SRCDIR)/%.cpp,$(OBJDIR)/%.o,$(SRC)) $(OBJ_CUDA)

$(OBJDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(@D)
	$(CC) -c $(STD) $(FLGS) $< -o $@

$(BIN): $(OBJ)
	$(CC) $(STD) $(FLGS) $^ -o $@


#bench:
#	${CC} ${STD} ${HAS_NVCC} ${FLGS} ${SRC} -o ${BIN}

run_bench:
	./${BIN} -b

run_daemon:
	./${BIN} -d

gprof:
	gprof ${BIN} gmon.out > gprof.txt

callgrind:
	valgrind --tool=callgrind ./${BIN}
	#callgrind_annotate callgrind.out

flamegraph:
	sudo perf record -g ./${BIN}
	sudo perf script | sudo ../FlameGraph/stackcollapse-perf.pl | sudo ../FlameGraph/flamegraph.pl > rpi.svg

docker_img:
	sudo docker build . -t benchmarks

docker_run:
	sudo docker run --privileged -it benchmarks:latest /bin/bash

gpu_docker_img:
	sudo nvidia-docker build . -t benchmarks

gpu_docker_run:
	sudo nvidia-docker run --privileged -it benchmarks:latest /bin/bash

avail_macros:
	gcc -dM -E - </dev/null

install:
	mv ${BIN} /usr/bin

uninstall:
	rm -f /usr/bin/${BIN}

clean:
	rm -f ${BIN}
	rm -f *.txt
	rm -f *.out
	rm -f *.data*
	rm -f *perf
	rm -f *.csv
	rm -f *.o
	rm -rf obj/
