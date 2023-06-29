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

typedef struct tInfo {
   int size;
   int new_size;
   int *data;
   int splitpoint;
} tInfo_t;

// static volatile int* groupCount;
// static volatile int* groupState;

// Not used, showed worse performance compared to omp barrier.
// volatile void specBarrier(int groupSize) {
//     int thread = omp_get_thread_num();
//     int group = thread / groupSize;
//     int mystate;
//     #pragma omp critical
//     {
//         mystate = groupState[group];
//         groupCount[group]++;
//         if (groupCount[group] == groupSize) {
//         groupCount[group] = 0;
//         groupState[group] = 1 - mystate;
//         }
//     }
//     while (mystate == groupState[group]);
// }

// Compare function used for qsort.
int cmpfunc(const void * a, const void * b) {
   return ( *(int*)a - *(int*)b );
}

void createChunks(int* array, const int size, tInfo_t* threadInfo, const int nThreads){
    int chunk = size / nThreads;
    int remainder = size - (chunk * nThreads);
    int i = 0;

    // Create chunks for each thread.
    for (int thread = 0; thread < nThreads; thread++){

        int start;
        int stop;
        start = i*chunk;

        if (thread < nThreads-1){
            stop = start + chunk;
            threadInfo[thread].size = stop - start;
        }
        else{
            stop = start + chunk + remainder;
            threadInfo[thread].size = stop - start;
        }
        threadInfo[thread].data = (int*) malloc(sizeof(int)*threadInfo[thread].size);

        i++;

        // Fill local data with data from unsorted array.
        for (int i = 0; i < threadInfo[thread].size; i++){
            threadInfo[thread].data[i] = array[i+start];
        }
    }
}

int median(int* data, const int size){
    int minIndex = 0;
    int maxIndex = size - 1;
    int midIndex = (minIndex + maxIndex) / 2;

    // Find median of sorted data.
    if (size % 2 == 1) {
        // Odd size
        return data[midIndex];
    } else { 
        // Even size
        return (data[midIndex] + data[midIndex + 1]) / 2;
    }
}

int findSplit(int* data, const int size, const int pivot){
    int low = 0;
    int high = size - 1;
    int mid;

    // Find the index in the array of the pivot using binary search O(log n), possible since data is sorted
    while (low <= high) {
        mid = (low + high) / 2;

        if (data[mid] == pivot) {
            return mid;
        } else if (data[mid] < pivot) {
            low = mid + 1;
        } else {
            high = mid - 1;
        }
    }
    return low;
}


int* mergeData(tInfo_t* data, tInfo_t* data2, const bool isUpper){
    int *array;
    int size;
    int i,j;
    int end_d1, end_d2;
    int k = 0;

    // Set correct values (start, stop) and allocate correct size of array if upper part of array.
    if (isUpper){
        i = data->splitpoint;
        j = data2->splitpoint;
        end_d1 = (data->size);
        end_d2 = (data2->size);
        size = (data->size - i) + (data2->size - j);;
        array = (int*)malloc(sizeof(int) * size);
    }
    // Set correct values (start, stop) and allocate correct size of array if lower part of array.
    else{
        i = 0;
        j = 0;
        end_d1 = (data->splitpoint);
        end_d2 = (data2->splitpoint);
        size = end_d1 + end_d2;
        array = (int*)malloc(sizeof(int) * size);
    }

    // Merge the upper/lower parts of old data into sorted new array.
    while(i < end_d1 && j < end_d2){
        if (data->data[i] < data2->data[j]){
            array[k] = data->data[i];
            i++;
        }
        else{
            array[k] = data2->data[j];
            j++;
        }
        k++;
    }
    // Check if remainders, if true, add them.
    while(i < end_d1 && k < size){
        array[k] = data->data[i];
        i++;
        k++;
    }
    while(j < end_d2 && k < size){
        array[k] = data2->data[j];
        j++;
        k++;
    }

    // Update new array size for next recursive call.
    data->new_size = size;

    return array;
}

void global_sort(tInfo_t* threadInfo, const int groupSize, const int nThreads, int* pivots){
    if (groupSize == 1) return;
    int thread = omp_get_thread_num();
    int locid = thread % groupSize;
    int group = thread/groupSize;

    // Calculate pivot for each group.
// Strategy 1:
    // if (locid == 0) pivots[group] = median(threadInfo[thread].data, threadInfo[thread].size);
    // #pragma omp barrier
    // threadInfo[thread].splitpoint = findSplit(threadInfo[thread].data, threadInfo[thread].size, pivots[group]);
    // #pragma omp barrier
    // pivots[group] = 0;

// // Strategy 2 modified:
    int pivot = 0;
    int groupStart = group * groupSize;
    for (int i = 0; i < groupSize; i++) {
        pivot += median(threadInfo[i + groupStart].data, threadInfo[i + groupStart].size);
    }
    pivot /= groupSize;
    threadInfo[thread].splitpoint = findSplit(threadInfo[thread].data, threadInfo[thread].size, pivot);
    // specBarrier(groupSize);
    #pragma omp barrier

//  Strategy 2:
    // pivots[group] += median(threadInfo[thread].data, threadInfo[thread].size);
    // #pragma omp barrier
    // int pivot = pivots[group]/groupSize;
    // threadInfo[thread].splitpoint = findSplit(threadInfo[thread].data, threadInfo[thread].size, pivot);
    // #pragma omp barrier
    // pivots[group] = 0;

// Strategy 3:
    // int medians[groupSize];
    // int groupStart = group * groupSize;

    // for (int i = 0; i < groupSize; i++) {
    //     medians[i] = median(threadInfo[i + groupStart].data, threadInfo[i + groupStart].size);
    // }
    // qsort(medians, groupSize, sizeof(int), cmpfunc);
    // int pivot = (medians[groupSize/2 - 1] + medians[(groupSize/2)]) / 2;
    // threadInfo[thread].splitpoint = findSplit(threadInfo[thread].data, threadInfo[thread].size, pivot);
    // #pragma omp barrier

    // Merge data between the groups into array. (False indicates lower part of the arrays (below splitpoint) True indicates the upper part of the arrays)
    int* array;
    if (locid < groupSize/2){
        array = mergeData(&threadInfo[thread], &threadInfo[thread+groupSize/2], false);
    }
    else{
        array = mergeData(&threadInfo[thread], &threadInfo[thread-groupSize/2], true);
    }
    #pragma omp barrier
    // specBarrier(groupSize);

    // Free old data and update local data for the current thread to the merged one and update the size of the array.
    free(threadInfo[thread].data);
    threadInfo[thread].data = array;
    threadInfo[thread].size = threadInfo[thread].new_size;
    #pragma omp barrier

    // Recursive call with halved group size.
    global_sort(threadInfo, groupSize/2, nThreads, pivots);
}


void pqsort(int* array, const int size, const int nThreads){
    if (size == 1){
        return;
    }
    tInfo_t* threadInfo = (tInfo_t*) malloc(sizeof(tInfo_t)*nThreads);

    // Divide data into p equal parts.
    createChunks(array, size, threadInfo, nThreads);

    // Sort chunks locally in parallel.
    #pragma omp parallel num_threads(nThreads)
    {
        int tid = omp_get_thread_num();
        qsort(threadInfo[tid].data, threadInfo[tid].size, sizeof(int), cmpfunc);
        #pragma omp barrier
    }

    // Create pivots array.
    int* pivots = (int*)calloc(nThreads, sizeof(int));

    // Perform global sort.
    #pragma omp parallel num_threads(nThreads)
    {
        global_sort(threadInfo, nThreads, nThreads, pivots);
    }

    // Put array back together (now fully sorted).
    int j = 0;
    for (int thread = 0; thread < nThreads; thread++){
        for (int i = 0; i < threadInfo[thread].size; i++){
            array[j] = threadInfo[thread].data[i];
            j++;
        }
    }

    // Free memory.
    for (int thread = 0; thread < nThreads; thread++){
        free(threadInfo[thread].data);
    }
    free(pivots);
    free(threadInfo);
}