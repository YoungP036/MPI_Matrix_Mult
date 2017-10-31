PGMS=mpi_mmult

all:	${PGMS}

mpi_mmult:	mpi_mmult.c
	mpicc -w -O3 -o mpi_mmult mpi_mmult.c
clean:
	rm -f ${PGMS}

