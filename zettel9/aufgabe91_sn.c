#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "mpi.h"

#define	TRUE		1
#define FALSE		0
#define	MASTER		0
#define TAG_A		2
#define TAG_B		3
#define	TAG			1
#define	UP			1
#define DOWN		-1
#define M_SIZE 		2
#define SUB_M_SIZE 	1
#define N_DIMS 		2

void vorrotation();
void gentleman();

MPI_Status status;
MPI_Comm commCart;
int dims[2] = {M_SIZE, M_SIZE};
int periods[N_DIMS] = {TRUE, TRUE}; // geht nur fuer N_DIMS=2?
int reorder = TRUE;
int	size, rank; 
int	coords[N_DIMS];
int myresult[3] = {0, 0, 0};
int c;

int *a, *b;	
	  
int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (size != N_DIMS*M_SIZE) {
		if (rank == MASTER)
			printf("Starte bitte mit -np %d\n", N_DIMS*M_SIZE);
		exit(1);
	}

	MPI_Cart_create(MPI_COMM_WORLD, N_DIMS, dims, periods, reorder, &commCart);
	MPI_Cart_coords( commCart, rank, N_DIMS, coords );
	
	a = (int*)(rank*2);
	b = (int*)(rank*2);	

	vorrotation();

	gentleman();
	
	if(rank == MASTER) {
		int globalresult[M_SIZE][M_SIZE];
		int tmpResult[3] = {0, 0, 0};
		int i;
		for(i = 0; i < (M_SIZE*M_SIZE)-1; i++)
		{
			MPI_Recv(&tmpResult, 3, MPI_INT, MPI_ANY_SOURCE, TAG, MPI_COMM_WORLD, &status);
			//printf("%d empfaengt %d. (%d,%d) c=%d\n", rank, i+1, tmpResult[0], tmpResult[1], tmpResult[2]);
			globalresult[tmpResult[0]][tmpResult[1]] = tmpResult[2];
		}
		globalresult[coords[0]][coords[1]] = c;
		
		printf("\nErgebnis:\n==========\n");
		for(i = 0; i < M_SIZE; i++)
		{
			int j;
			for(j = 0; j < M_SIZE; j++)
			{
				printf("%d ", globalresult[i][j]);
			}
			printf("\n");
		}
	}
	else {
		MPI_Send(&myresult, 3, MPI_INT, MASTER, TAG, MPI_COMM_WORLD);
		//printf("%d sendet (%d,%d) c=%d\n", rank, myresult[0], myresult[1], myresult[2]);
	}
	
	MPI_Finalize();
	return 0;
}

void vorrotation(){
	//MPI_Barrier(commCart);
	printf("Vor Vorrotation: Koordinaten von %d sind (%d,%d). Wert von a = %d, Wert von b = %d\n", rank, coords[0] + 1, coords[1] + 1, a, b);
	MPI_Barrier(commCart);
	
	int rank_source, rank_dest;
	
	// Zeilenweise verschieben
	MPI_Cart_shift(commCart, 1, DOWN*coords[0], &rank_source, &rank_dest);
	
	/*int coordsS[N_DIMS], coordsD[N_DIMS];
	MPI_Cart_coords(commCart, rank_source, N_DIMS, coordsS);
	MPI_Cart_coords(commCart, rank_dest, N_DIMS, coordsD);
	MPI_Barrier(commCart);
	printf("Self: (%d, %d), receive from: (%d, %d), send to: (%d, %d)\n", coords[0] + 1, coords[1] + 1, coordsS[0] + 1, coordsS[1] + 1, coordsD[0] + 1, coordsD[1] + 1);
	MPI_Barrier(commCart);*/
	
	int *tmpA;
    MPI_Sendrecv(&a, 1, MPI_INT, rank_dest, TAG, &tmpA, 1, MPI_INT, rank_source, TAG, commCart, &status);
	a = tmpA;
	printf("Nach Zeilenrotation: Koordinaten von %d sind (%d,%d). Wert von a = %d\n", rank, coords[0] + 1, coords[1] + 1, a);
	MPI_Barrier(commCart);
	
	
	// Spaltenweise verschieben
	MPI_Cart_shift(commCart, 0, DOWN*coords[1], &rank_source, &rank_dest);
	
	int coordsS[N_DIMS], coordsD[N_DIMS];
	MPI_Cart_coords(commCart, rank_source, N_DIMS, coordsS);
	MPI_Cart_coords(commCart, rank_dest, N_DIMS, coordsD);
	MPI_Barrier(commCart);
	printf("Self: (%d, %d), receive from: (%d, %d), send to: (%d, %d)\n", coords[0] + 1, coords[1] + 1, coordsS[0] + 1, coordsS[1] + 1, coordsD[0] + 1, coordsD[1] + 1);
	MPI_Barrier(commCart);
	
	int *tmpB;
    MPI_Sendrecv(&b, 1, MPI_INT, rank_dest, TAG, &tmpB, 1, MPI_INT, rank_source, TAG, commCart, &status);
	b = tmpB;
	printf("Nach Spaltenrotation: Koordinaten von %d sind (%d,%d). Wert von b = %d\n", rank, coords[0] + 1, coords[1] + 1, b);
	MPI_Barrier(commCart);	
}

void gentleman(){
	c = 0;
	int i;
	int rank_source, rank_dest;
	myresult[0] = coords[0];
	myresult[1] = coords[1];
	
	for(i = 1; i <= M_SIZE; i++){
		c += ((int)a)*((int)b);
		MPI_Cart_shift(commCart, 1, DOWN, &rank_source, &rank_dest);
		int *tmpA;
	    MPI_Sendrecv(&a, 1, MPI_INT, rank_dest, TAG, &tmpA, 1, MPI_INT, rank_source, TAG, commCart, &status);
		a = tmpA;
		
		int coordsS[N_DIMS], coordsD[N_DIMS];
		MPI_Cart_coords(commCart, rank_source, N_DIMS, coordsS);
		MPI_Cart_coords(commCart, rank_dest, N_DIMS, coordsD);
		MPI_Barrier(commCart);
		printf("Zeilen: Self: (%d, %d), receive from: (%d, %d), send to: (%d, %d), c = %d\n", coords[0] + 1, coords[1] + 1, coordsS[0] + 1, coordsS[1] + 1, coordsD[0] + 1, coordsD[1] + 1, c);
		MPI_Barrier(commCart);
		
		MPI_Cart_shift(commCart, 0, DOWN, &rank_source, &rank_dest);
		int *tmpB;
	    MPI_Sendrecv(&b, 1, MPI_INT, rank_dest, TAG, &tmpB, 1, MPI_INT, rank_source, TAG, commCart, &status);
		b = tmpB;		
		
		MPI_Cart_coords(commCart, rank_source, N_DIMS, coordsS);
		MPI_Cart_coords(commCart, rank_dest, N_DIMS, coordsD);
		MPI_Barrier(commCart);
		printf("Spalten: Self: (%d, %d), receive from: (%d, %d), send to: (%d, %d), c = %d\n", coords[0] + 1, coords[1] + 1, coordsS[0] + 1, coordsS[1] + 1, coordsD[0] + 1, coordsD[1] + 1, c);
		MPI_Barrier(commCart);
	}
	
	myresult[2] = c;
	//printf("Ergebnis (%d,%d): %d\n", coords[0] + 1, coords[1] + 1, c);
}