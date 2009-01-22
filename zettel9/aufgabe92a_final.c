/*
Aufgabe 9.2 a)
Implementierung:
Die Implementierung ist so gewählt, dass jeder Prozess ein Teilarray des gesamten Arrays besitzt, welches er auch mit Zufallswerten initialisiert. 
Über Zeilen- und Spaltenkommunikatoren werden per MPI_Allgather die Werte der anderen Prozesse geholt und sortiert. Jeder Prozess behält die
für sich relevanten Daten.

Aufbau für Kommunikation (Beispiel):

   	 0    1    2    3    4    5
  |----|----|----|----|----|----|
0 |			     |			    |
1 |	   Pr #0	 |	  Pr #1	    |	
2 |  		     |			    |
  |----|----|----|----|----|----|
3 |			     |			    |
4 |	   Pr #2     |	  Pr #3	    |	
5 |  		     |			    |
  |----|----|----|----|----|----|

*/

#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define N 12 
#define ROOT 0

void initData(int **data,int ratio);
void printDataSet(int **data, int ratio, int my_rank);
int compare_asc (const void * a, const void * b);
int compare_desc (const void * a, const void * b);

int main(argc, argv) 
	int argc; 
char **argv;
{ 		
	int size_all, my_rank, color_row, color_col;
	int recvbuf[N];

	MPI_Comm 	comm_row, comm_col;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size_all);
	MPI_Comm_rank(MPI_COMM_WORLD, &my_rank);
	
	int p_dim = (int)sqrt(size_all); 

	int ratio = N / p_dim; // Verhältnis Zeilen / Spalten zu Prozessen

	if(pow(p_dim,2) != size_all){
		if(my_rank == ROOT) printf("P = %d muss eine Quadratzahl sein!\n", size_all);
		MPI_Finalize();
		return 1;
	}
	if(N % p_dim != 0 && size_all % p_dim != 0){
		if(my_rank == ROOT) printf("N = %d und P = %d müssen jeweils durch %d teilbar sein!\n", N, size_all, p_dim);
		MPI_Finalize();
		return 1;
	}	

	// Subarray mit Zufallswerten initialisieren 
	int **data = (int **) malloc(ratio * sizeof(int *));
	int i;
	for(i = 0; i < ratio; i++) {
		data[i] = (int *) malloc(ratio * sizeof(int));
	}
	srand((int)((unsigned)time(NULL)*my_rank));
	initData(data,ratio);

	// Anfangszustand ausgeben
	if(my_rank == ROOT)printf("------------ Anfangszustand: ------------\n");
	MPI_Barrier(MPI_COMM_WORLD);
	printDataSet(data,ratio, my_rank);

	// Kommunikatoren initialisieren
	color_row = my_rank / p_dim;
	color_col = my_rank % p_dim;
	MPI_Comm_split(MPI_COMM_WORLD, color_row, my_rank, &comm_row);
	MPI_Comm_split(MPI_COMM_WORLD, color_col, my_rank, &comm_col);

	int z;
	int phase_max = log(N)/log(2); // Anzahl Phasen für Shearsort ermitteln
	int *col_array = (int *)malloc(ratio*sizeof(int)); 	// Spaltenarray erstellen
	
	for(z = 0; z <= phase_max; z++) // log N + 1 Phasen 
	{

	// Zeilen sortieren
		for(i = 0; i < ratio; i++)
		{
			// Zeilen einsammeln
			MPI_Allgather(data[i], ratio, MPI_INT, recvbuf, ratio, MPI_INT, comm_row);

			// Zeilen sortieren
			if((color_row*ratio + i) % 2 == 0){
				qsort(recvbuf,N,sizeof(int),compare_asc);
			} else{
				qsort(recvbuf,N,sizeof(int),compare_desc);
			}

			// Passende Werte speichern
			int k;
			for(k = 0; k < ratio; k++){
				data[i][k] = recvbuf[color_col*ratio + k];
			}

		}
		if(z == phase_max){ 
			break;	//In der letzten Phase werden nur Zeilen sortiert
		}

		MPI_Barrier(MPI_COMM_WORLD);

	// Spalten sortieren

		for(i = 0; i < ratio; i++)
		{
		// Spalten in Spaltenarray laden
			int j;
			for(j = 0; j < ratio; j++)
			{
				col_array[j]=data[j][i];
			}

		// Spalten einsammeln
			MPI_Allgather(col_array, ratio, MPI_INT, recvbuf, ratio, MPI_INT, comm_col);

		// Spalten sortieren
			qsort(recvbuf,N,sizeof(int),compare_asc);

			// Passende Werte speichern
			int k;
			for(k = 0; k < ratio; k++){
				data[k][i] = recvbuf[color_row*ratio + k];
			}
		}
		MPI_Barrier(MPI_COMM_WORLD);
		
	}
	if(my_rank == ROOT)printf("--------------- Ergebnis: ---------------\n");
	MPI_Barrier(MPI_COMM_WORLD);

	printDataSet(data,ratio, my_rank);	

	MPI_Comm_free(&comm_row);
	MPI_Comm_free(&comm_col);
	
	free(col_array);
	for(i = 0; i < ratio; i++) {
		free(data[i]);
	}	
	free(data);
	MPI_Finalize();
	return 0;
}
int compare_asc (const void * a, const void * b){
	return ( *(int*)a - *(int*)b );
}
int compare_desc (const void * a, const void * b){
	return ( *(int*)b - *(int*)a );
}

void initData(int **data, int ratio){
	int i, j;

	for(i = 0; i < ratio; i++){		
		for(j = 0; j < ratio; j++){
			data[i][j] = rand() % 100 + 1;
				//printf("(%d,%d) = %d \n", i, j-1, data[i][j]);
		}
	}
}

void printDataSet(int **data, int ratio, int my_rank){
	printf("Dataset von Prozess %d: (%d x %d)\n", my_rank, ratio, ratio);
	int j, i;
	for(i = 0; i < ratio; i++){
		for(j = 0; j < ratio; j++){
			printf("%d\t", data[i][j]);
		}
		printf("\n");
	}
}

