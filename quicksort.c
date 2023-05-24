#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

int* createUnsortedArray(int size, int max){
    int* array = (int*)malloc(sizeof(int)*size);
    srand(time(NULL));

    for (int i = 0; i < size; i++){
        array[i] = rand() % max;
    }

    // for (int i = 0; i < size; i++){
    //     printf("Value: %d\n", array[i]);
    // }

    return array;
}

int* sort(int* array, int size){
    return array;
}

int main(int argc, char *argv[])
{   
    if (argc != 4){
        printf("Incorrect number of arguments, correct: Array Size, Max Randomized Number, Static Array?\n");
        return 0;
    }
    // Gather neccessary information about array to be sorted.
    int size = atoi(argv[1]);
    int max = atoi(argv[2]);
    bool stat = atoi(argv[3]);
    int* array;

    // If array is static, then used pre-set values, otherwise do it fully random of size 'size' and maximum possible value 'max'.
    if (stat){
        size = 17;
        max = 531;
        array = (int[]){1,32,6,41,6,278,43,365,7,53,2,531,7,98,3,6,467};
    }
    else{
       array = createUnsortedArray(size, max);
    }

    // Make the call to sort the array:
    int sorted = sort(array, size);

    // Unallocated unused memory.
    free(array);
    return 0;
}