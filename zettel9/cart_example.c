#include <mpi.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/param.h>

#define	TRUE		1
#define FALSE		0
#define	MASTER		0
#define	TAG			1
#define	UP			1
#define DOWN		-1

char*	PrintTopoType(int, char*);

int main(int argc, char *argv[])
{
MPI_Status		status;
MPI_Group		MpiCommGroup;
MPI_Comm		CartComm;
char			hostname[80];
int				i, j;
int 			rank, nTasks, CartRank, TranslateRank;
int				SourceRank, DestRank;

int				nDims = 2;
int				Dims[2] = {2, 2};
int				MpiDims[2];
int				Periods[2] = {FALSE, FALSE};
int				Coord[2];
int				Neighbors[2];
int				Send, Recv, Sum, TopoType;
int				*rDims, *rPeriods, *rCoords;
char 			String[80];

	MPI_Init(&argc, &argv);

	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Comm_size(MPI_COMM_WORLD, &nTasks);
	if (nTasks<4) {
	    if (rank==0) {
	         fprintf(stderr,"This Program needs at least 4 Processes.\n");
	    }     
	    MPI_Finalize();
	    return -1;
	}         
	gethostname(hostname, 79);
	printf("%-15.12s: MPI_COMM_WORLD rank %d\n", hostname, rank);

	/* get proposals from MPI for balanced cartesian topologys */
	nDims = 2;
	MpiDims[0] = MpiDims[1] = 0;
	MPI_Dims_create(nTasks, nDims, MpiDims);
	if (rank == MASTER) {
		printf("Proposal from MPI for %d Nodes and %d Dims : ", nTasks, nDims);
		for (i = 0; i < nDims; i++)
			printf("%d ", MpiDims[i]);
		printf("\n");
	}

	/* 	if we decide the size of a dimension (MpiDims[i] <> 0), 
		MPI find out the rest. Only those entries where
		MpiDims[i] = 0 are
		modified by the call
		errornous statements results in :
			Can not partition nodes as requested :
			Invalid arguments to MPI routine
			Aborting program!
			Segmentation Fault (core dumped)
	*/
	nDims = 2;
	MpiDims[0] = 1;
	MpiDims[1] = 0;
	MPI_Dims_create(nTasks, nDims, MpiDims);
	if (rank == MASTER) {
		printf("Proposal from MPI for %d Nodes and %d Dims : ",
		       nTasks, nDims);
		for (i = 0; i < nDims; i++)
			printf("%d ", MpiDims[i]);
		printf("\n");
	}

	MPI_Cart_create(MPI_COMM_WORLD, nDims, Dims, Periods, TRUE, &CartComm);

	/*Retrieve the cartesian topology */
	if (rank == MASTER) {
		MPI_Topo_test(CartComm, &TopoType);
		printf ("Topology type is %s\n", PrintTopoType(TopoType, String));
	}

	MPI_Cartdim_get(CartComm, &nDims);
	rDims 	 = (int*) malloc (nDims * sizeof(int));
	rPeriods = (int*) malloc (nDims * sizeof(int));
	rCoords  = (int*) malloc (nDims * sizeof(int));
	MPI_Cart_get(CartComm, nDims, rDims, rPeriods, rCoords);
	printf("%-15.12s: Tology informations : Dims = %d\n", hostname, nDims);
	for (i = 0; i < nDims ; i++) 
		printf("%-15.12s: Processes in Dim[%d] = %d  Periodicity = %d  my Coordinates = %d\n",\
 		hostname, i, rDims[i], rPeriods[i], rCoords[i]);


	MPI_Comm_rank(CartComm, &CartRank);
	/* translate rank -> coordinates */
	MPI_Cart_coords(CartComm, CartRank, nDims, Coord);
	/* translate back, coordinates -> rank */
	MPI_Cart_rank(CartComm, Coord, &TranslateRank);

	printf("%-15.12s: CartComm rank = %d  x = %d  y = %d  TranslateRank = %d\n", \
		hostname, CartRank, Coord[0], Coord[1], TranslateRank);

	/* find out ranks of the neighbors */
	for (i = 0; i < nDims; i++) {
		MPI_Cart_shift(CartComm, i, UP, &SourceRank, &DestRank);
		if (DestRank != MPI_PROC_NULL) 
			Neighbors[i] = DestRank;
		MPI_Cart_shift(CartComm, i, DOWN, &SourceRank, &DestRank);
		if (DestRank != MPI_PROC_NULL) 
			Neighbors[i] = DestRank;
	}

	/* handmade global sum on virtual hypercube */
	Sum = Send = CartRank;
	for (i = 0; i < nDims; i++) {
			MPI_Sendrecv(&Sum,  1, MPI_INT, Neighbors[i], TAG, 
						 &Recv, 1, MPI_INT, Neighbors[i], TAG, CartComm, &status);
		Sum += Recv;
	}
	printf("%-15.12s: Sum %d\n", hostname, Sum);

	MPI_Finalize();
	return 0;
}

char* PrintTopoType(int tt, char *str)
{
	if (str != NULL)
	    switch (tt) {
		case MPI_UNDEFINED : strcpy(str, "MPI_UNDEFINED"); break;
		case MPI_GRAPH	   : strcpy(str, "MPI_GRAPH"); break;
		case MPI_CART	   : strcpy(str, "MPI_CART"); break;
	    }
	return str;
}
