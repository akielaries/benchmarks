#CC		= g++-10

# USE G++-10 for baremetal testing, G++-12 for Docker use
ifeq ($(shell command -v g++-12 2>/dev/null),)
	CC = g++-10
else
	CC = g++-12
endif

FLGS	= -std=c++20 -march=native -pg -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare
SRC		= primes.cpp montecarlo.cpp benchmark.cpp sys.cpp -lpthread
BIN		= BENCHMARK

bench:
	${CC} ${FLGS} ${SRC} -o ${BIN}

run:
	./${BIN}

gprof:
	gprof ${BIN} gmon.out > gprof.txt

callgrind:
	valgrind --tool=callgrind ./${BIN}
	#callgrind_annotate callgrind.out

flamegraph:
	sudo perf record -g ./${BIN}
	sudo perf script | sudo ../FlameGraph/stackcollapse-perf.pl | sudo ../FlameGraph/flamegraph.pl > rpi.svg

build_img:
	sudo docker build . -t benchmarks

run_container:
	sudo docker run -it benchmarks:latest /bin/bash

clean:
	rm -f ${BIN}
	rm -f *.txt
	rm -f *.out
	rm -f *.data*
	rm -f *perf

