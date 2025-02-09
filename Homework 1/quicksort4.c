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

int main() {  // 50 elements
    int arr[] = {438, 921, 372, 856, 129, 584, 734, 612, 892, 267, 
    783, 492, 356, 975, 168, 453, 729, 501, 624, 387, 
    839, 247, 693, 182, 519, 864, 302, 741, 625, 573, 
    934, 672, 159, 428, 956, 721, 583, 267, 839, 498, 
    192, 745, 678, 215, 487, 603, 827, 593, 951, 362}; 
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
