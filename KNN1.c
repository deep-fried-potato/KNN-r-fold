#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<time.h>
#include<math.h>
#include "seeds_meta.h"

typedef struct point{
  int classlabel;
  float attribs[dim];
  float distance;
}point;

void swap (point *a, point *b){
    point temp = *a;
    *a = *b;
    *b = temp;
}

void randomize ( point arr[], int n ){
  for (int i = n-1; i > 0; i--){
    int j = rand() % (i+1);
    swap(&arr[i], &arr[j]);
  }
}

void get_data(point pointArray[]){
  FILE * fp;
  char buf[1000];
  char *token;
  int attribcount;
  int pointcount;


  fp = fopen("seeds_dataset.txt","r");
  pointcount = 0;

  while (fgets(buf , 1000 , fp)) {
    token = strtok(buf, "\t\n");
      attribcount = 0;
      while( token != NULL ) {
        if(attribcount<dim) pointArray[pointcount].attribs[attribcount] = atof(token);
        else pointArray[pointcount].classlabel = atoi(token);
        token = strtok(NULL, "\t\n");
        attribcount++;
      }
    pointcount++;
  }
}
float distance(point pointA,point pointB,int p){
  float sum = 0;
  for (int i = 0; i <dim; i++) {
    sum = sum + pow(fabs(pointA.attribs[i]-pointB.attribs[i]),p);
  }
  sum = pow(sum,1.0/p);
  return sum;
}


void get_distances(point trainArray[],point testPoint,int p,int r){
  for (int i = 0; i < N-N/r; i++) {
    trainArray[i].distance=distance(trainArray[i],testPoint,p);
  }
}

int pointCompare(const void * pointA,const void * pointB){
  float a = ((point *)pointA)->distance;
  float b = ((point *)pointB)->distance;
  if (a>b) return 1;
  else return -1;
}

int KNN(point trainArray[],point testArray[],int k,int p,int r){
  int corrects=0;
  for (int i = 0; i <N/r; i++) {
    get_distances(trainArray,testArray[i],p,r);
    qsort(trainArray,N-N/r,sizeof(trainArray[0]),pointCompare);
    int classlabels[classes] = {0};
    for (int j = 0; j < k; j++) {
      classlabels[trainArray[j].classlabel-1]++;
    }
    int maxidx  = 0;
    for (int j = 0; j < classes; j++) {
      if (classlabels[j]>classlabels[maxidx]) maxidx = j;
    }
    if (maxidx+1 == testArray[i].classlabel) corrects++;
  }
  return corrects;
}

int KNN_r_fold(point pointArray[],int k,int p,int r){
  point trainArray[N-N/r];
  point testArray[N/r];
  int testCount;
  int trainCount;
  int totalCorrect = 0;
  for (int i = 0; i < r; i++) {
    testCount=0;
    trainCount=0;
    for (int j = 0; j < N; j++) {
      if(j>=(i*N)/r && j < ((i+1)*N)/r){
        testArray[testCount] = pointArray[j];
        testCount++;
      }
      else{
        trainArray[trainCount] = pointArray[j];
        trainCount++;
      }
    }
    totalCorrect+=KNN(trainArray,testArray,k,p,r);
  }
  //printf( "K:%d  P:%d Corrects:%d  Accuracy: %f\n",k,p,totalCorrect,(float)totalCorrect/N );
  return totalCorrect;
}

int main(void){

    srand((unsigned int) time(NULL));

    point pointArray[N];

    get_data(pointArray);
    randomize(pointArray,N);
    int maxCorrect = 0;
    int temp= 0 ;
    int optK=0;
    int optP =0;
    for (int k = 1; k < 10; k++) {
      for (size_t p = 1; p < 5; p++) {
        temp = KNN_r_fold(pointArray,k,p,10);
        if(temp>maxCorrect){
          maxCorrect= temp;
          optK = k;
          optP = p;
        }
      }
    }
    printf("\nOptimum Values:\n K:%d P:%d Accuracy:%f\n",optK,optP,(float)maxCorrect/N );

return 0;
}
