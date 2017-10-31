# Matrix Multiplication via MPI

  Implements MPI to compute the inner product of the two matrices using multiple hosts. Supports any
  matrix dimensionality capable of producing inner product, as well as any number of hosts. 
  
  I broadcast the entirety of matrix B to all hosts, and then iteratively send single row slices of matrix A
  to an individual host. That host will then compute a one row slice of the final answer and send it back
  to the master host. Finally the master will assemble all of these slices into a coherant final output
  and print it out.
