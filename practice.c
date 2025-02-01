// Practicing with writing time executions for parallel programs

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>  // For gettimeofday()


#define NUM_THREADS 2 // Number of threads defined for this program.

// Function to measure time

double read_timer() {
  struct timeval time; 
  gettimeofday(&time, NULL); 
  return time.tv_sec + time.tv_usec * 1.0e-6;
}

// Dummy function for the thread
void *worker(void *arg) {
    int thread_id = *(int *)arg;
    printf("Thread %d is running.\n", thread_id);
    return NULL;
}

int main() {
  pthread_t threads[NUM_THREADS]; 
  int threads_ids[NUM_THREADS]; 

  double start_time = read_timer();
  
  // Create threads 
  for(int i = 0; i < NUM_THREADS; i++){
    threads_ids[i] = i; 
    pthread_create(&threads[i], NULL, worker, &threads_ids[i]); 

    // Wait for threads to finish: 
  for (int i = 0; i < NUM_THREADS; i++) {
      pthread_join(threads[i], NULL); 
    }
    // Read the end time after all threads have finished: 
    double end_time = read_timer(); 

    // Calculate and print the execution time: 
    printf("Execution time: %f seconds\n", end_time - start_time);
    return 0; 
}
