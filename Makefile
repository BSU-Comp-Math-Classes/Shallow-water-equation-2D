CC =gcc
CFLAGS= -Wall -lm
MPICC=mpicc
CUDACC = nvcc

swe_2d_cuda: swe_2d.cu
	$(CUDACC) -o swe_2d_cuda swe_2d.cu

swe_2d: swe_2d.c
	$(CC) $(CFLAGS) -o swe_2d swe_2d.c

clean:
	rm -f swe_2d

