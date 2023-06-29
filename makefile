CFLAGS=-Wall -fopenmp -g
LDFLAGS=-lm -O3 -ffast-math -march=native -fopt-info-vec
INCLUDES=-I.

test: test.o pquicksort.o
	gcc $(CFLAGS) -o test test.o pquicksort.o $(LDFLAGS)

pquicksort.o: pquicksort.c pquicksort.h
	gcc -c pquicksort.c $(CFLAGS) $(INCLUDES)

test.o: test.c pquicksort.h
	gcc -c test.c $(CFLAGS) $(INCLUDES)

clean:
	rm -f test *.o