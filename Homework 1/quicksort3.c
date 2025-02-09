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

int main() { // 500 elements
    int arr[] = {938, 345, 12, 908, 721, 412, 678, 920, 837, 456, 124, 687, 954, 182, 395, 234, 827, 634, 985, 102, 
    593, 738, 854, 920, 682, 341, 980, 812, 462, 590, 658, 746, 297, 910, 452, 382, 648, 219, 723, 846, 
    174, 365, 985, 732, 692, 912, 654, 380, 567, 792, 593, 426, 675, 823, 192, 485, 639, 738, 950, 273,
    618, 738, 129, 483, 926, 184, 673, 849, 247, 682, 294, 135, 683, 972, 184, 653, 295, 826, 273, 654,
    839, 172, 465, 792, 356, 938, 573, 486, 275, 681, 124, 873, 246, 285, 937, 198, 395, 823, 275, 638,
    473, 965, 384, 712, 592, 834, 472, 318, 528, 689, 192, 725, 645, 839, 719, 835, 274, 957, 728, 614,
    781, 524, 392, 173, 927, 746, 283, 198, 593, 785, 432, 981, 394, 583, 728, 192, 634, 917, 659, 284,
    365, 892, 467, 589, 812, 657, 918, 394, 684, 723, 185, 284, 729, 927, 318, 754, 286, 945, 765, 892,
    293, 768, 512, 938, 472, 184, 678, 572, 895, 172, 685, 924, 512, 734, 264, 783, 529, 817, 345, 627,
    836, 273, 586, 792, 368, 726, 937, 192, 632, 748, 127, 392, 821, 482, 284, 679, 523, 827, 164, 356,
    839, 685, 719, 748, 623, 835, 285, 947, 834, 586, 523, 968, 745, 283, 682, 832, 586, 732, 942, 856,
    473, 692, 824, 153, 629, 574, 287, 729, 486, 823, 372, 947, 763, 914, 672, 829, 314, 578, 752, 492,
    528, 791, 473, 582, 825, 192, 678, 942, 287, 569, 729, 831, 692, 573, 498, 834, 285, 617, 845, 743,
    728, 965, 578, 293, 891, 742, 638, 193, 748, 628, 839, 749, 327, 456, 183, 748, 264, 589, 726, 832,
    293, 745, 563, 785, 917, 672, 835, 725, 648, 582, 926, 739, 829, 473, 927, 698, 738, 328, 476, 852,
    829, 562, 738, 495, 827, 185, 573, 732, 472, 925, 746, 263, 682, 583, 748, 527, 184, 629, 583, 486,
    927, 648, 275, 924, 736, 812, 639, 738, 489, 928, 374, 592, 745, 389, 926, 682, 356, 938, 839, 654,
    839, 275, 927, 687, 825, 374, 528, 924, 573, 673, 264, 586, 825, 728, 285, 756, 826, 743, 629, 932,
    837, 482, 573, 672, 934, 472, 736, 623, 874, 582, 745, 926, 825, 467, 739, 682, 927, 374, 293, 754,
    573, 926, 384, 478, 592, 837, 274, 832, 475, 923, 829, 472, 573, 264, 586, 927, 825, 174, 729, 847}; 

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
