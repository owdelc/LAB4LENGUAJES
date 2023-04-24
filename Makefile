# Usage:
# make        # compile all binary
# make clean  # remove ALL binaries and objects

.PHONY = all clean

CC = g++			# compiler to use

LINKERFLAG = -lm

STANDARD = -std=c++17
INCLUDE = -I./headers
PERMISSIVE = -Wwrite-strings

compile:
	@echo "Compile..."
	${CC}  main.cpp src/*.cpp  -o main.o ${STANDARD} ${INCLUDE} 

log:
	@echo "Compile..."
	${CC}  main.cpp src/*.cpp -o mainlog.o ${STANDARD} ${INCLUDE} -D logYard

yalex:
	./main.o
	${CC}  scanner.cpp src/*.cpp  -o scanner.o ${STANDARD} ${INCLUDE}
	./scanner.o -f "foo.txt"

clean:
	@echo "Cleaning up..."
	rm -rvf *.o ${BINS}