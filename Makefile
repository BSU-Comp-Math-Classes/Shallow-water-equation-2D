CC =gcc
CFLAGS= -Wall -lm
MPICC=mpicc

swe_2d: swe_2d.c
	$(CC) $(CFLAGS) -o swe_2d swe_2d.c

clean:
	rm -f swe_2d

