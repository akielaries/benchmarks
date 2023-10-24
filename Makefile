CC		= g++
FLGS	= -std=c++20 -march=native -pg -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare
SRC		= primes.cpp montecarlo.cpp benchmark.cpp
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

clean:
	rm -f ${BIN}
	rm -f *.txt
	rm -f *.out
	rm -f *.data*
	rm -f *perf

