CFLAGS=-Wall
INCLUDES=-I/opt/X11/include
LDFLAGS=-L/opt/X11/lib -lX11 -lm -O3 -g -ffast-math -march=native -fopt-info-vec

quicksort: quicksort.o
	gcc $(CFLAGS) $(INCLUDES) -o quicksort quicksort.o $(LDFLAGS)

galsim.o: quicksort.c
	gcc -c quicksort.c $(LDFLAGS)

clean:
	rm -f *.o