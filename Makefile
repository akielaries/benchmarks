CC		= g++
FLGS	= -std=c++20 -march=native -pg -g -Wall -Wextra -pedantic -Wno-unused-result -Wparentheses -Wsign-compare
SRC		= primes.cpp benchmark.cpp
BIN		= BENCHMARK

bench:
	${CC} ${FLGS} ${SRC} -o ${BIN}

run:
	./${BIN}

clean:
	rm -f ${BIN}
	rm -f *.txt
	rm -f *.out

