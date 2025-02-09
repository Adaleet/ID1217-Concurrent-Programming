#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <time.h>

void swap(int* a, int* b) {
    int temp = *a;
    *a = *b;
    *b = temp;
}

int partition(int array[], int low, int high) {
    int pivot = array[high];
    int i = low;

    for (int j = low; j < high; j++) {
        if (array[j] < pivot) {
            swap(&array[i], &array[j]);
            i++;
        }
    }
    swap(&array[i], &array[high]);
    return i;
}

void quicksort(int array[], int low, int high) {
    if (low < high) {
        int pivot_index = partition(array, low, high);

        if (high - low < 100) {
            quicksort(array, low, pivot_index - 1);
            quicksort(array, pivot_index + 1, high);
        } else {
            #pragma omp parallel
            {
                #pragma omp single nowait
                {
                    #pragma omp task
                    quicksort(array, low, pivot_index - 1);

                    #pragma omp task
                    quicksort(array, pivot_index + 1, high);
                }
            }
        }
    }
}

int compare_doubles(const void* a, const void* b) {
    double diff = *(double*)a - *(double*)b;
    if (diff > 0) return 1;
    if (diff < 0) return -1;
    return 0;
}

int main() {
    int sizes[] = {100000, 1000000, 10000000}; // different sizes of the array with significant difference in size
    int num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    int num_threads[] = {1, 2, 3, 4};
    int num_thread_counts = sizeof(num_threads) / sizeof(num_threads[0]);
    const int NUM_RUNS = 5;

    for (int i = 0; i < num_sizes; i++) {
        int n = sizes[i];
        int* array = (int*)malloc(n * sizeof(int));
        int* array_copy = (int*)malloc(n * sizeof(int));

        srand(time(NULL));
        for (int j = 0; j < n; j++) {
            array[j] = rand() % n;
        }
        printf("Array size: %d\n", n);

        double sequential_time = 0.0;

        for (int t = 0; t < num_thread_counts; t++) {
            int threads = num_threads[t];
            omp_set_num_threads(threads); // openMP, specifying the number of threads from the array

            double execution_times[NUM_RUNS];

            for (int run = 0; run < NUM_RUNS; run++) {
                for (int j = 0; j < n; j++) {
                    array_copy[j] = array[j];
                }
                double start_time = omp_get_wtime(); // measuring only the parallelized part as give the instruction
                quicksort(array_copy, 0, n - 1);
                double end_time = omp_get_wtime();
                execution_times[run] = end_time - start_time;
            }

            // Effective way to choose the median value is to sort it using the unbuilt qsort, and calculate the median.
            qsort(execution_times, NUM_RUNS, sizeof(double), compare_doubles);
            double median_time = execution_times[NUM_RUNS / 2];

            if (threads == 1) {
                sequential_time = median_time; // Sequential time (number of threads is 1) recorded for the speedup = "sequential time / parallelized time" 
            }

            double speedup = sequential_time / median_time;
            printf("  Threads: %d, Median Time: %f seconds, Speedup: %f\n", threads, median_time, speedup);
        }
        free(array);
        free(array_copy);
    }
    return 0;
}