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
int periods[N_DIMS] = {TRUE, TRUE};
int reorder = TRUE;
int	size, rank; 
int	coords[N_DIMS];

int *a, *b;	
	  
int main(int argc, char *argv[]){
	MPI_Init(&argc, &argv);
  	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);
	MPI_Cart_create(MPI_COMM_WORLD, N_DIMS, dims, periods, reorder, &commCart);
	MPI_Cart_coords( commCart, rank, N_DIMS, coords );

	a = (int*)(rank*2);
	b = (int*)(rank*2);	

	vorrotation();

	gentleman();
	
	MPI_Finalize();
	return 0;
}

void vorrotation(){
	MPI_Barrier(commCart);
	printf("Vor Vorrotation: Koordinaten von %d sind (%d,%d). Wert von a = %d, Wert von b = %d\n", rank, coords[0] + 1, coords[1] + 1, a, b);
	MPI_Barrier(commCart);
	
	int rank_source, rank_dest;
	
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
	int c = 0;
	int i;
	int rank_source, rank_dest;
	
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
	
	printf("Ergebnis (%d,%d): %d\n", coords[0] + 1, coords[1] + 1, c);
}