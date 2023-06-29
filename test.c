#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <float.h>
#include <string.h>
#include <pthread.h>
#include <omp.h>
#include <limits.h>
#include "pquicksort.h"


int compare(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void createUnsortedArray(int* array, int* test, const int size, const int max){
    srand(time(NULL));

    // Add randomized element to both arrays.
    for (int i = 0; i < size; i++){
        int val = rand() % max;
        array[i] = val;
        test[i] = val;
    }
}

int main(int argc, char *argv[])
{   
    if (argc != 4){
        printf("Incorrect number of arguments, correct: Array Size, Max Randomized Number, Number of Threads.\n");
        return 0;
    }
    // Gather neccessary information about array to be sorted.
    int size = atoi(argv[1]);
    int max = atoi(argv[2]);
    int nThreads = atoi(argv[3]);

    int* array = (int*)malloc(sizeof(int)*size);
    int* test = (int*)malloc(sizeof(int)*size);
    createUnsortedArray(array, test, size, max);

    // Sort array using C library, used for testing that the final array is actually sorted.
    qsort(test, size, sizeof(int), compare);

    // Time execution of sorting the array.
    double start; 
    double end; 
    start = omp_get_wtime();

    pqsort(array, size, nThreads);

    end = omp_get_wtime();
    printf("%f seconds\n", end - start);

    // Check with library sort if the sorting was indeed correct.
    bool sorted = true;
    for (int i = 0; i < size; i++){
        if (test[i] != array[i]){
            printf("\nNot sorted correctly..\n");
            sorted = false;
            break;
        }
    }
    if (sorted) printf("Sorted Correctly.\n");

    // Unallocated unused memory.
    free(array);
    free(test);
    return 0;
}