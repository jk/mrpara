#include <stdlib.h>
#include <stdio.h>
#include <mpi.h>
#include <math.h>

#include "aufgabe91.h"

#define ROOT 0
#define TAG_DEFAULT 0

MPI_Comm 	comm_row, comm_col;
MPI_Group	group, group_row, group_col;
int nprocs, myid;

int main (int argc, char const *argv[])
{
	//int **data;
	int data[][];
	
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &nprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &myid);
	
	printf("#Prozessoren: %d\n", nprocs);
	
	MPI_Finalize();
	return 0;
}