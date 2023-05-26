#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>

// Source: https://www.youtube.com/watch?v=WprjBK0p6rw&ab_channel=CuriousWalk

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

void swap(int* x, int* y)
{
    // Store value in temporary integer.
    int temp = *x;
    // Set the value of x with the value of y.
    *x = *y;
    // Use the temporary value to set value of y.
    *y = temp;
}

void sort_partition(int* array, int start, int stop){
    if (stop-start <= 1){
        return;
    }

    int toSwap = start-1;
    int pivot = array[stop-1];
    for (int i = start; i < stop; i++){
        if (array[i] <= pivot){
            toSwap = toSwap + 1;
            if (i > toSwap){
                swap(&array[i], &array[toSwap]);
            }
        }
    }

    sort_partition(array, 0, toSwap);
    sort_partition(array, toSwap, stop);
}

int* global_sort(int* array, int size){
    if (size == 1){
        return array;
    }

    int pivot = array[size-1];
    int toSwap = -1;
    for (int i = 0; i < size; i++){
        if (array[i] <= pivot){
            toSwap = toSwap + 1;
            if (i > toSwap){
                swap(&array[i], &array[toSwap]);
            }
        }
    }

    sort_partition(array, 0, toSwap);
    sort_partition(array, toSwap, size);

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
        size = 10;
        max = 9;
        array = (int[]){3,2,5,0,1,8,7,6,9,4};
    }
    else{
       array = createUnsortedArray(size, max);
    }

    for (int i = 0; i < size; i++){
        printf("%d, ", array[i]);
    }
    printf("\n");
    printf("\n");

    // Make the call to sort the array:
    printf("PIVOT: %d\n", array[size-1]);
    printf("\n");
    int* sorted = global_sort(array, size);

    for (int i = 0; i < size; i++){
        printf("%d, ", sorted[i]);
    }
    printf("\n");

    // Unallocated unused memory.
    if (!stat){
        free(array);
    }
    return 0;
}