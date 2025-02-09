#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

#define THRESHOLD 2  // Lower for testing parallel behavior

typedef struct {
    int *arr;
    int low;
    int high;
} QuickSortArgs;

void swap(int *a, int *b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int arr[], int low, int high) {
    int pivot = arr[high];  
    int i = low - 1;

    for (int j = low; j < high; j++) {
        if (arr[j] < pivot) {
            i++;
            swap(&arr[i], &arr[j]);  
        }
    }
    swap(&arr[i + 1], &arr[high]);
    return i + 1;  
}

void *parallel_quicksort(void *args) {
    QuickSortArgs *qsArgs = (QuickSortArgs *)args;
    int *arr = qsArgs->arr;
    int low = qsArgs->low;
    int high = qsArgs->high;

    if (low < high) {
        int pivot_index = partition(arr, low, high);

        if ((high - low) > THRESHOLD) {
            pthread_t left_thread, right_thread;
            
            QuickSortArgs *left = malloc(sizeof(QuickSortArgs));
            QuickSortArgs *right = malloc(sizeof(QuickSortArgs));

            if (!left || !right) {
                perror("Memory allocation failed");
                exit(1);
            }

            left->arr = arr;
            left->low = low;
            left->high = pivot_index - 1;
            right->arr = arr;
            right->low = pivot_index + 1;
            right->high = high;

            pthread_create(&left_thread, NULL, parallel_quicksort, left);
            pthread_create(&right_thread, NULL, parallel_quicksort, right);
            pthread_join(left_thread, NULL);
            pthread_join(right_thread, NULL);
        } else {
            QuickSortArgs *left = malloc(sizeof(QuickSortArgs));
            QuickSortArgs *right = malloc(sizeof(QuickSortArgs));

            if (!left || !right) {
                perror("Memory allocation failed");
                exit(1);
            }

            left->arr = arr;
            left->low = low;
            left->high = pivot_index - 1;
            right->arr = arr;
            right->low = pivot_index + 1;
            right->high = high;

            parallel_quicksort(left);
            parallel_quicksort(right);
        }
    }

    free(qsArgs); 
    return NULL;
}

double read_timer() { 
    struct timeval time; 
    gettimeofday(&time, NULL);
    return time.tv_sec + time.tv_usec * 1.0e-6; 
}
void printArray(int arr[], int size) {
    for (int i = 0; i < size; i++)
        printf("%d ", arr[i]);
    printf("\n");
}

int main() {

    int arr[] = { // 10 elements
    10, 80, 30, 90, 40, 50, 70, 20, 60, 100, 
    5, 95, 35, 85, 25, 75, 45, 55, 65, 15, 
    11, 99, 33, 77, 22, 88, 44, 66, 20, 98}; 

    /*int arr[] = { // 100 elements
    10, 80, 30, 90, 40, 50, 70, 20, 60, 100, 
    5, 95, 35, 85, 25, 75, 45, 55, 65, 15, 
    11, 99, 33, 77, 22, 88, 44, 66, 20, 98, 
    12, 81, 31, 91, 41, 51, 71, 21, 61, 101, 
    6, 96, 36, 86, 26, 76, 46, 56, 67, 17, 
    13, 97, 37, 87, 27, 78, 47, 58, 68, 19, 
    14, 82, 32, 92, 42, 52, 72, 22, 62, 102, 
    7, 94, 38, 84, 24, 74, 43, 54, 64, 16, 
    18, 83, 39, 93, 49, 53, 79, 23, 63, 103, 
    9, 89, 34, 73, 48, 57, 69, 29, 59, 109};*/

    int n = sizeof(arr) / sizeof(arr[0]);
    printf("Unsorted array:\n");
    printArray(arr, n);

    QuickSortArgs *args = malloc(sizeof(QuickSortArgs));
    if (!args) {
        perror("Memory allocation failed");
        return 1;
    }

    args->arr = arr;
    args->low = 0;
    args->high = n - 1;

    double start_time = read_timer();

    pthread_t sorting_thread;
    pthread_create(&sorting_thread, NULL, parallel_quicksort, args);
    pthread_join(sorting_thread, NULL); // Ensuring that the worker threads are terminated, in order to measure the end time.
    double end_time = read_timer();

    printf("Sorted array:\n");
    printArray(arr, n);
    printf("Execution time: %.10f seconds\n", end_time - start_time);  

    return 0;
}
