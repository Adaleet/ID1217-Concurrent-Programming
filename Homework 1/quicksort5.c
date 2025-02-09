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

int main() { // 200 elements
    int arr[] = {934, 581, 362, 725, 493, 218, 726, 482, 695, 842,
    367, 821, 154, 639, 275, 531, 798, 406, 573, 918, 
    762, 382, 659, 237, 694, 829, 425, 978, 216, 568, 
    591, 673, 892, 743, 159, 862, 403, 317, 674, 429, 
    719, 483, 852, 712, 951, 684, 297, 806, 528, 739, 
    375, 861, 491, 279, 532, 897, 214, 693, 583, 729, 
    418, 672, 873, 359, 652, 714, 267, 394, 827, 719, 
    136, 951, 395, 812, 629, 784, 328, 519, 872, 739, 
    562, 978, 456, 621, 839, 527, 691, 354, 785, 492, 
    918, 246, 392, 715, 836, 584, 928, 472, 759, 315, 
    673, 457, 629, 879, 238, 495, 671, 823, 591, 314, 
    837, 751, 198, 396, 873, 562, 482, 695, 172, 721, 
    739, 583, 291, 652, 319, 741, 896, 473, 589, 427, 
    819, 532, 795, 627, 891, 253, 479, 638, 731, 879, 
    465, 928, 273, 692, 584, 457, 752, 193, 678, 816, 
    953, 495, 732, 579, 843, 284, 527, 816, 594, 379, 
    912, 269, 643, 752, 489, 326, 592, 472, 639, 856, 
    719, 285, 867, 359, 692, 752, 539, 891, 726, 517, 
    498, 821, 582, 716, 395, 928, 689, 623, 792, 318, 
    732, 427, 621, 743, 986, 312, 457, 795, 319, 715}; 
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
