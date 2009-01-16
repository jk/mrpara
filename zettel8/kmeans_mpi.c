#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <mpi.h>

#define BUFSIZE	64
#define ROOT 0
#define MYTAG 0
#define CTRLTAG 1
int	buf[BUFSIZE];
int *data;
int *cluster_ctrs;
static int n, m, k;
MPI_Status status;


/*int eudklid(int* vecA, int* vecB);
void initData(int** data);
void chooseClusterCenters(int** data, int* cluster_ctrs);
int isClusterCenter(int* cluster_ctrs, int cluster);
void assignElements(int** data, int* cluster_ctrs);*/
void printDataSet();
/*
int* clusterElements(int cluster_nr);
void assignNewClusters(int** data, int* cluster_ctrs);
*/

int main (int argc, char *argv[]){
	int	size, rank;
	MPI_Status	status;
	
	if (argc != 4) {
		printf("Anzahl der Parameter muessen 3 sein.\n");
		return 1;
	}

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &size);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	n = atoi(argv[1]);
	m = atoi(argv[2]);
	k = atoi(argv[3]);
	
	if (k >= n) {
	   printf("Es gibt nichts zu clustern. k >= n (k: %d n: %d)\n", k, n);
	   return 1;
    }

	int i;
	
	srand((unsigned)time(NULL));
	
	if(rank == ROOT) {
		printf("Anzahl Prozesse: %d\n", size);
		int nsize = (int) (n*m)/size;
        data = (int *) calloc(n * (m+1), sizeof(int *));

        printDataSet();

		MPI_Bcast(&nsize, 1, MPI_INT, MYTAG, MPI_COMM_WORLD);

		int done = 0, counter = 0;
		while(done || MPI_Recv(0, 0, MPI_INT, MPI_ANY_SOURCE, CTRLTAG, MPI_COMM_WORLD, &status) == MPI_SUCCESS) {
			MPI_Send(0, 0, MPI_INT, status.MPI_SOURCE, CTRLTAG, MPI_COMM_WORLD);
			int recvData[10];
			MPI_Recv(recvData, 10, MPI_INT, status.MPI_SOURCE, MYTAG, MPI_COMM_WORLD);
			
			int i;
			for(i = 0; i < 10; i++)
				if(counter-1 < n*(m+1))
					data[counter++] = tmpData[i];
				else
					done = 1;
		}
		printf("Daten initalisiert:\n");
		printDataSet();
        //MPI_Recv(data, n*(m+1), MPI_INT, MPI_ANY_SOURCE, MYTAG, MPI_COMM_WORLD, &status);
	}
	else {
		MPI_Request req;
		MPI_Isend(0, 0, MPI_INT, ROOT, CTRLTAG, MPI_COMM_WORLD);
		int masterFlag = -1;
		
		while(MPI_Recv(masterFlag, 1, MPI_INT, ROOT, CTRLTAG, MPI_COMM_WORLD)) {
		
			int tmpSend[10];
			int i;
			for(i = 0; i < 10; i++)
			{
				tmpSend[i] = rand() % 100 + 1;
			}
			MPI_Send(tmpSend, 10, MPI_INT, ROOT, MYTAG, MPI_COMM_WORLD);
			// Gibt's noch Arbeit?
			MPI_Isend(0, 0, MPI_INT, ROOT, CTRLTAG, MPI_COMM_WORLD, &req);
		}
		/*
        int colCount = (int) (n / (size - 1));
        if(rank < (n % size)-1){
            colCount++;
        }
        printf("ColCount: %d", colCount);
        for(i = 0; i < colCount; i++) {
            int* column = (int *) calloc((m + 1), sizeof(int *));
            column[0] = -1;
            int j;
            for(j = 1; j < m + 1; j++){
                column[j] = rand() % 100 + 1;
            }
            //MPI_Send(column, m+1, MPI_INT, ROOT, MYTAG, MPI_COMM_WORLD);
        }
		*/	
	}



	
	if(rank == ROOT){
        if(data == NULL){
            printf("Konnte keinen Speicher besorgen");
            return 1;
        }


	   /* array für die speicherung der clusterzentren */
	   //cluster_ctrs = (int *) malloc(k*sizeof(int));
	
	   printDataSet();
    }
	
	MPI_Finalize();
	/* erste zufällige auswahl von clusterzentren	
	chooseClusterCenters(data, cluster_ctrs);
	
	printf("ClusterCenters: ");
	for(i = 0; i < k; i++){
	   printf("%d ", cluster_ctrs[i]);
	}
	printf("\n");
	
	printf("Assign clusters\n");
	/*Elemente den Clustern zuordnen
	assignElements(data, cluster_ctrs);
	printDataSet();
	
    for(i = 0; i < k; i++){
        printf("Center of cluster %d is %d, size %d\n", i, cluster_ctrs[i], sizeOfCluster(i));
    }
    int* elements;
    for(i = 0; i < k; i++){
        printf("Vectors in cluster %d: ", i);
        elements = clusterElements(i);
        int j = 0;
        int size = sizeOfCluster(i);
        for(j = 0; j < size; j++){
            printf("%d ", elements[j]);
        }
        printf("\n");
    }
    
    /*Neue Clusterzentren bestimmen
    assignNewClusters(data, cluster_ctrs);
    flag = 0;
    printf("New Clustercenters: ");
    for(i = 0; i < k; i++){
        printf("%d ", cluster_ctrs[i]);
    } 
    printf("\n");
    
    assignElements(data, cluster_ctrs);
    printDataSet();
    
    redistributeElements(data, cluster_ctrs);
    printDataSet();
	
	free(data);*/
	return 0;
}

void printDataSet(){
	printf("Dataset: (%d x %d)\n", n, m);
	int j;
	for(j = 0; j < n*(m+1); j++){
        printf("%d\t", data[j]);
        if(j % n == 0){
            printf("\n");
        }
	}
}

/*
void chooseClusterCenters(int** data, int* cluster_ctrs){
    int pot_cluster_ctr;
	int j = 0;
	while(j < k){
		pot_cluster_ctr = rand()%n;
		if(isClusterCenter(cluster_ctrs, pot_cluster_ctr) == 0){
			data[pot_cluster_ctr][0] = pot_cluster_ctr;
			cluster_ctrs[j] = pot_cluster_ctr;
			j++;
		}
	}
}


int euklid(int* vecA, int* vecB){
	int i;
	double sum = 0;
	
	for(i = 0; i < m; i++)
		sum += pow((*(vecA + sizeof(int)*i) - *(vecB + sizeof(int)*i)), 2);
	
	return (int) sqrt(sum);
}


int isClusterCenter(int* cluster_ctrs, int pot_cluster_ctr){
	int i, contains = 0;
	for(i = 0; i < k; i++){
		if(cluster_ctrs[i] == pot_cluster_ctr){
			contains = 1;
		}
	}
	return contains;
}


int* clusterElements(int cluster_nr){
    int cluster_size = sizeOfCluster(cluster_nr);
    int* cluster_elements = (int*)calloc(cluster_size, sizeof(int));
    
    int i;
    int j = 0;
    for(i = 0; i < n; i++){
        if(data[i][0] == cluster_ctrs[cluster_nr]){
            cluster_elements[j++] = i;
        }
    }
    
    return cluster_elements;
}

void assignElements(int** data, int* cluster_ctrs){
    int minDistance, CenterPosition, tmpDistance;
    int i,j;
    
    for(i = 0; i < n;i++){
        minDistance = euklid(data[i],data[cluster_ctrs[0]]);
        CenterPosition = cluster_ctrs[0];
        for(j=1; j < k;j++){
            tmpDistance = euklid(data[i],data[cluster_ctrs[j]]);
            if(tmpDistance < minDistance){
                minDistance = tmpDistance;
                CenterPosition = cluster_ctrs[j];
            }
        }

        data[i][0] = CenterPosition;
    }
}

void assignNewClusters(int** data, int* cluster_ctrs){
    int i,j,l, tmpDistance;
    for(i = 0; i < k; i++){
    
        int* cluster_elements = clusterElements(i);
        int cluster_size = sizeOfCluster(i);
        int minDistance = 0;
            for(l = 0; l < cluster_size; l++){
                minDistance += euklid(data[cluster_elements[l]], data[cluster_elements[0]]);
            }
        int newCenterPosition = cluster_elements[0];
        

        for(j = 1; j < cluster_size; j++){
            tmpDistance = 0;
            for(l = 0; l < cluster_size; l++){
                tmpDistance += euklid(data[cluster_elements[l]], data[cluster_elements[j]]);
            }
            if(tmpDistance < minDistance){
                minDistance = tmpDistance;
                newCenterPosition = cluster_elements[j];
            }
        } 
        cluster_ctrs[i] = newCenterPosition;
    }
    
}

int sizeOfCluster(int cluster_nr) {
    if(cluster_nr >= k && cluster_nr < 0)
        return -1;
        
    int i;
    int c = 0;
    for(i = 0; i < n; i++) {
        if (cluster_ctrs[cluster_nr] == data[i][0])
            c++;
    }
    return c;
}


void printDataSet(){
	printf("Dataset: (%d x %d)\n", n, m);
	int j, i;
	for(j = 0; j <= m; j++){
    	for(i = 0; i < n; i++){
			printf("%d\t", data[i][j]);
		}
		printf("\n");
	}
}

void redistributeElements(int** data, int* cluster_ctrs){
    int* old_dist = data[0];
    assignNewClusters(data, cluster_ctrs);
    assignElements(data, cluster_ctrs);
    
    int* new_dist = data[0];
    if(distsEqual(old_dist, new_dist) == 1){
        redistributeElements(data, cluster_ctrs);
    }
}

int distsEqual(int* old_dist, int* new_dist){
    int i;
    for(i = 0; i < n; i++){
        if(old_dist[i] != new_dist[i])
            return 1;
    }
    return 0;
}
*/