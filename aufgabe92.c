#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define N 20
#define M 20

int data[N][M];

int main (int argc, char const *argv[])
{
	int size, rank;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	
//	MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
	
	MPI_Finalize();
	return 0;
}