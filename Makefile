PGMS=my_mmult matrix_times_vector my_mmult2

all:	${PGMS}

matrix_times_vector: 	matrix_times_vector.c
	mpicc -w -O3 -o matrix_times_vector matrix_times_vector.c mmult.c
mmult.o:	mmult.c
	gcc -w -c -O3 mmult.c

my_mmult:	my_mmult.c
	mpicc -w -O3 -o my_mmult my_mmult.c mmult.c
my_mmult2:	my_mmult2.c
	mpicc -w -O3 -o my_mmult2 my_mmult2.c mmult.c

clean:
	rm -f ${PGMS}

