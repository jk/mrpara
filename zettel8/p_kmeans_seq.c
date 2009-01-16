#include <stdlib.h>
#include <stdio.h>
#include <math.h>

int **data;
int *cluster_ctrs;
static int n, m, k;


int eudklid(int* vecA, int* vecB);
void initData(int** data);
void chooseClusterCenters(int** data, int* cluster_ctrs);
int isClusterCenter(int* cluster_ctrs, int cluster);
void assignElements(int** data, int* cluster_ctrs);
void printDataSet();
int* clusterElements(int cluster_nr);
void assignNewClusters(int** data, int* cluster_ctrs);

int main (int argc, char const *argv[]){
	if (argc != 4) {
		printf("Anzahl der Parameter muessen 3 sein.\n");
		return 1;
	}
	
	n = atoi(argv[1]);
	m = atoi(argv[2]);
	k = atoi(argv[3]);
	
	if (k >= n) {
	   printf("Es gibt nichts zu clustern. k >= n (k: %d n: %d)\n", k, n);
	   return 1;
    }
	
	
	/* genügend speicherplatz besorgen, um n vektoren der grösse m speichern zu können */
	data = (int **) malloc(n * sizeof(int *));
	int i;
	for(i = 0; i < n; i++) {
		data[i] = (int *) malloc((m + 1)* sizeof(int));
		/* In data[i][0] wird die n-Position des zugehörigen Clusters gespeichert*/
		data[i][0] = -1;
	}
	
	if(data == NULL){
		printf("Konnte keinen Speicher besorgen");
		return 1;
	}

	/* array für die speicherung der clusterzentren */
	cluster_ctrs = (int *) malloc(k*sizeof(int));
	
	/* zufallszahlen generator initialisieren */
	srand((unsigned)time(NULL));
	
	/* zufallsdatensatz erstellen */
	initData(data);
	
	printDataSet();

	
	/* erste zufällige auswahl von clusterzentren */	
	chooseClusterCenters(data, cluster_ctrs);
	
	printf("ClusterCenters: ");
	for(i = 0; i < k; i++){
	   printf("%d ", cluster_ctrs[i]);
	}
	printf("\n");
	
	printf("Assign clusters\n");
	/*Elemente den Clustern zuordnen*/
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
    
    redistributeElements(data, cluster_ctrs);
    printDataSet();
	free(data);
	return 0;
}

/**
 * Initalisiert ein Dataset mit zufälligen Werten.
 */
void initData(int** data){
	int i, j;
	for(i = 0; i < n; i++){
		for(j = 1; j <= m; j++){
			data[i][j] = rand() % 100 + 1;
			//printf("(%d,%d) = %d \n", i, j-1, data[i][j]);
		}
	}
}

/**
 * Wählt im ersten Schritt zufällig Vektoren als Clusterzentren aus.
 */
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

/**
 * Berechnet den euklidischen Abstand zwischen zwei Vektoren.
 */
int euklid(int* vecA, int* vecB){
	int i;
	double sum = 0;
	
	for(i = 0; i < m; i++)
		sum += pow((*(vecA + sizeof(int)*i) - *(vecB + sizeof(int)*i)), 2);
	
	return (int) sqrt(sum);
}

/**
 * Überprüft ob der übegebene Vektor bereits in Clusterzentrum ist.
 */
int isClusterCenter(int* cluster_ctrs, int pot_cluster_ctr){
	int i, contains = 0;
	for(i = 0; i < k; i++){
		if(cluster_ctrs[i] == pot_cluster_ctr){
			contains = 1;
		}
	}
	return contains;
}

/**
 * Sucht zu einer Clusternummer die zugehörigen Vektoren raus
 */
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

/**
 * Ordnet die Elemente den vorhandenen Clustern zu
 */
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

/**
 * Bestimmt neue Clusterzentren
 */
void assignNewClusters(int** data, int* cluster_ctrs){
    int i,j,l, tmpDistance;
    for(i = 0; i < k; i++){
    
        int* cluster_elements = clusterElements(i);
        int cluster_size = sizeOfCluster(i);
        /* erste Element*/
        int minDistance = 0;
            for(l = 0; l < cluster_size; l++){
                minDistance += euklid(data[cluster_elements[l]], data[cluster_elements[0]]);
            }
        int newCenterPosition = cluster_elements[0];
        
        /* alle anderen Elemente*/
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

/**
 * Errechnet die Anzahl an Elementen in einem übergebenem Cluster (Nummer des Clusters)
 */
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
