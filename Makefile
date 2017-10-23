PGMS=my_mmult

all:	${PGMS}

mmult.o:	mmult.c
	gcc -w -c -O3 mmult.c

my_mmult:	my_mmult.c
	mpicc -w -O3 -o my_mmult my_mmult.c mmult.c

clean:
	rm -f ${PGMS}

