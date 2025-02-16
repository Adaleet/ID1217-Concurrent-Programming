#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <fcntl.h>  // For O_CREAT
#include <unistd.h> 

#define H 10   
#define N 5  
int pot = 0;  

sem_t *mutex;   
sem_t *full_pot;    
sem_t *empty_pot;   

void* bee_thread(void* id) {
    int bee_id = *(int*)id;
    free(id);  // Free allocated memory
    while (1) {
        sleep(rand() % 3 + 1); 
        sem_wait(mutex); 
        while (pot >= H) { 
            sem_post(mutex); 
            sem_wait(empty_pot);  
            sem_wait(mutex);  
        }
        pot++;  
        printf("Bee %d added honey. Pot now has %d/%d portions.\n", bee_id, pot, H);
        if (pot == H) {
            printf("Bee %d filled the pot! Waking up the bear...\n", bee_id);
            sem_post(full_pot);  // Notify bear
        }
        sem_post(mutex);  
    }
}
void* bear_thread(void* arg) {
    while (1) {
        sem_wait(full_pot); 
        sem_wait(mutex);  
        printf("Bear wakes up! Eating all the honey...\n");
        pot = 0;  
        sleep(2); 
        printf("Bear goes back to sleep. Pot is now empty.\n");
        for (int i = 0; i < H; i++) {
            sem_post(empty_pot);
        }
        sem_post(mutex); 
    }
}
int main() {
    pthread_t bees[N], bear;

    mutex = sem_open("/mutex", O_CREAT, 0644, 1);
    full_pot = sem_open("/full_pot", O_CREAT, 0644, 0);
    empty_pot = sem_open("/empty_pot", O_CREAT, 0644, 0);

    pthread_create(&bear, NULL, bear_thread, NULL);

    for (int i = 0; i < N; i++) {
        int* bee_id = malloc(sizeof(int)); // dynamically allocate memory for each
        *bee_id = i + 1;
        pthread_create(&bees[i], NULL, bee_thread, bee_id);
    }

    for (int i = 0; i < N; i++) {
        pthread_join(bees[i], NULL);
    }

    sem_close(mutex);
    sem_close(full_pot);
    sem_close(empty_pot);
    sem_unlink("/mutex");
    sem_unlink("/full_pot");
    sem_unlink("/empty_pot");

    return 0;
}
