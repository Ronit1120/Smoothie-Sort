#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include"leak_detector_c.h"

/*
Written by Ronit Mahalmani
*/

//global file pointers
FILE *infile, *outfile; 

//point struct
typedef struct point{
  int x;
  int y;
}point;

//function prototypes
point* readData(int numShops);
int compareTo(point* ptrPt1, point*ptrPt2);
void mergeSort(point shopLocations[], int startIndex, int endIndex, int threshold);
void insertionSort(point shopLocations[], int length);
void sort(point shopLocations[], int numShops, int threshold);
void merge(point shopLocations[], int left, int right, int mid);
int searchShops(point shopLocations[], point query, int numShops);

//global variable for my location
point myPosition;

//function that returns a pointer to the shop locations to be sorted
point* readData(int numShops){
  //dma array that has size of the number of shops in the test case
  point* shopPoints = malloc(numShops * sizeof(point));

  //stores the coordinates from the input file to the array 
  
  for(int i = 0; i < numShops; i++){
    fscanf(infile, "%d %d", &shopPoints[i].x, &shopPoints[i].y);
  }
  return shopPoints;
}

//+ if ptr1 is farther, - if ptr1 is closer
int compareTo(point* ptrPt1, point*ptrPt2){
  //finds distance from my position to each pointPtr
  int distance1 = sqrt( pow( (ptrPt1->x - myPosition.x) , 2) + pow( (ptrPt1->y - myPosition.y) , 2) );
  int distance2 = sqrt( pow( (ptrPt2->x - myPosition.x) , 2) + pow( (ptrPt2->y - myPosition.y) , 2) );

  //if statements to meet the criteria on the rubric
  if(distance1 < distance2){
    return -1;
  }
  
  if(distance1 > distance2){
    return 1;
  }
  
  if(distance1 == distance2 ){
    if(ptrPt1->x < ptrPt2->x){
      return -1;
    }
    else if(ptrPt1->x > ptrPt2->x){
      return 1;
    }
    else if(ptrPt1->y < ptrPt2->y){
      return -1;
    }
    else if(ptrPt1->y > ptrPt2->y){
      return 1;
    }
    else if(ptrPt1->x == ptrPt2->x && ptrPt1->y == ptrPt2->y){
      return 0;
    }
  }

}


void insertionSort(point shopLocations[], int length){
  //varibale for loops
  int i,j;
  
  for(i = 0; i < length; i++){

    //temp point for comparison
    point temp = shopLocations[i];

    //loop for previous points
    for(j = i-1; j>=0; j--){
      if (compareTo( &shopLocations[j], &temp ) == 1 ){ //compares previous points to i point and then shitfs it to the correct index if needed
        shopLocations[j+1] = shopLocations[j];
      }
      else{
        break;
      }
      
    }
    //actually assigns the correct point to the correct index
    shopLocations[j+1] = temp;
    
  }
}

void sort(point* shopLocations, int numShops, int threshold){
  mergeSort(shopLocations, 0, (numShops - 1), threshold);
}

void mergeSort(point* shopLocations, int left, int right, int threshold){
  if(left < right){
    int mid = (left + right) / 2;
    mergeSort(shopLocations, left, mid, threshold);
    mergeSort(shopLocations, mid + 1, right, threshold);
    merge(shopLocations, left, right, mid);
    
    
    //splits array til the threshold array size is reached and then calls insertion sort
    if(right - left <= threshold){ 
      insertionSort(shopLocations, (right - left));
    }
    
  }
}

void merge(point shopLocations[], int left, int right, int mid){
  //tracker & loop variables
  int i, j, k;

  //variables for size of left and right arrays
  int leftSize = mid - left + 1;
  int rightSize = right - mid;

  point* leftArray = malloc(leftSize * sizeof(point));
  point* rightArray = malloc(rightSize * sizeof(point));

  //assigns values from original array to left and right arrays
  for(i = 0; i < leftSize; i++){
    leftArray[i].x = shopLocations[left + i].x;
    leftArray[i].y = shopLocations[left + i].y; 
  }
  
  for(j = 0; j < rightSize; j++){
    rightArray[j].x = shopLocations[mid + 1 + j].x;
    rightArray[j].y = shopLocations[mid + 1 + j].y; 
  }

  i = 0;//left tracker
  j = 0;//right tracker
  k = left;//original array tacker

  //adds elements to original array from left and right array depending on which one has the smaller distance
  while(i < leftSize && j < rightSize){
    if( compareTo(&leftArray[i], &rightArray[j]) == -1 ){
      shopLocations[k] = leftArray[i];
      i++;
      k++;
    }
    else{
      shopLocations[k] = rightArray[j];
      j++;
      k++;
    }
  }
  
  //loops for the trackers if they go out of bounds and there are remaining elements of either arrays
  while(i<leftSize){
    shopLocations[k] = leftArray[i];
    i++;
    k++;
  }
  while (j < rightSize){
    shopLocations[k] = rightArray[j];
    j++;
    k++;
  }

  free(leftArray);
  free(rightArray);
  
}

int searchShops(point shopLocations[], point query, int numShops){
  int left = 0;
  int right = numShops - 1;
  int mid;

  while(left <= right){
    mid = (left + right) / 2;

    if(compareTo(&shopLocations[mid], &query) == 0){
      return mid;
    }

    if(compareTo(&query, &shopLocations[mid]) == 1){
      left = mid + 1;
    }

    else{
      right = mid - 1;
    }
  }
  return -1;
}



int main(void) {
  atexit(report_mem_leak);// for memory leak stuff
  
  int numShops, numPointsSearch, threshold;
  point* shopLocations;
  point* queries;

  infile = fopen("in.txt", "r");
  outfile = fopen("out.txt", "w");

  fscanf(infile,"%d %d %d %d %d",&myPosition.x, &myPosition.y, &numShops, &numPointsSearch, &threshold);

  //pointer to array of shop locations to be sorted
  shopLocations = readData(numShops);

  //pointer to array of queries to be binary searched
  queries = readData(numPointsSearch);

  //calls wrapper function to sort the store locations
  sort(shopLocations, numShops, threshold);

  

  //loop that prints sorted points to console and out text file
  for(int i = 0; i < numShops; i++){
    printf("%d %d\n",shopLocations[i].x, shopLocations[i].y);
    fprintf(outfile, "%d %d\n",shopLocations[i].x, shopLocations[i].y);
  }

  //loop to call binary search function on each of the elements of the queries array
  for(int l = 0; l < numPointsSearch; l++){
    int searchRes = searchShops(shopLocations, queries[l], numShops);//binary search call for each query element
    //displays otput based on index if found
    if(searchRes == -1){
      printf("%d %d not found\n", queries[l].x, queries[l].y);
      fprintf(outfile,"%d %d not found\n", queries[l].x, queries[l].y);
    }
    else{
      //passes result + 1 for binary search return value because the index starts at 0 instead of 1
      printf("%d %d found at rank %d\n", queries[l].x, queries[l].y, (searchRes + 1));
      fprintf(outfile, "%d %d found at rank %d\n", queries[l].x, queries[l].y, (searchRes + 1));
    }
    
  }
  


  free(queries);
  free(shopLocations);
  
  return 0;
}