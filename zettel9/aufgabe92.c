#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define N 5 // Spalten
#define M 5 // Zeilen
#define ROOT 0

void initData(int** data);
void printDataSet(int** data);

int main (int argc, char const *argv[])
{
	int **data;
	int size_all, my_rank;
	MPI_Comm 	comm_row, comm_col;
    MPI_Group	group, group_row, group_col;
	
	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size_all);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	if(my_rank == ROOT){
	/* genügend speicherplatz besorgen, um n vektoren der grösse m speichern zu können */
		data = (int **) malloc(N * sizeof(int *));
		int i;
		for(i = 0; i < N; i++) {
			data[i] = (int *) malloc(M* sizeof(int));
		}
	//Datenarray mit Zufallswerten initialisieren
		srand((unsigned)time(NULL));
		initData(data);
		printDataSet(data);
	}	
	
	MPI_Comm_group(MPI_COMM_WORLD, &group);
	
	int p_row = size_all*M/(M+N); // Anzahl der Prozesse für Zeilen
	int p_col = size_all - p_row; // Anzahl der Prozesse für Spalten
	int i;
	//printf("Anzahl Prozesse für Spalten: %d, Reihen: %d\n",p_col,p_row );
	
	// Gruppe und Kommunikator für Zeilen erstellen
	int* ranks_row = (int *) malloc(p_row*sizeof(int));
	for(i = 0; i < p_row; i++)
	{
		ranks_row[i] = i;
		//printf("row[%d]:rank:%d\n",i,i);
	}
	MPI_Group_incl(group, p_row, ranks_row, &group_row);
	MPI_Comm_create(MPI_COMM_WORLD, group_row, &comm_row);
	free(ranks_row);
	
	// Gruppe und Kommunikator für Spalten erstellen
	int* ranks_col = (int *) malloc(p_col*sizeof(int));
	for(i = 0; i < p_col; i++)
	{
		ranks_col[i] = p_row + i;
		//printf("col[%d]:rank:%d\n",i,p_row+i);
	}
	MPI_Group_incl(group, p_col, ranks_col, &group_col);
	MPI_Comm_create(MPI_COMM_WORLD, group_col, &comm_col);
	free(ranks_col);
	
	// Werte holen
	MPI_Barrier(MPI_COMM_WORLD);

//	MPI_Comm_free(&comm_row);
//	MPI_Comm_free(&comm_col);

	
//	MPI_Allgather(sendbuf, sendcount, sendtype, recvbuf, recvcount, recvtype, comm);
//	MPI_Barrier(MPI_COMM_WORLD);
	MPI_Group_free(&group_row);
	MPI_Group_free(&group_col);
	MPI_Group_free(&group);
	MPI_Finalize();

	if (my_rank == ROOT){
		free(data);
	}
	return 0;
}

void initData(int** data){
	int i, j;
	for(i = 0; i < N; i++){
		for(j = 0; j <= M; j++){
			data[i][j] = rand() % 100 + 1;
			//printf("(%d,%d) = %d \n", i, j-1, data[i][j]);
		}
	}
}

void printDataSet(int** data){
	printf("Dataset: (%d x %d)\n", N, M);
	int j, i;
	for(j = 0; j <= M; j++){
    	for(i = 0; i < N; i++){
			printf("%d\t", data[i][j]);
		}
		printf("\n");
	}
}

