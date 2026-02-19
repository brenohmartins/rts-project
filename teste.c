#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <semaphore.h>

#define BUFFER_SIZE 2
#define NUM_MAQS 2

typedef struct{
    int id;
    sem_t start;
    sem_t done;
} Machine;

Machine machines[NUM_MAQS];
pthread_t machine_threads[NUM_MAQS];  // Array to store machine threads

sem_t B_items, B_slots;
sem_t B_count;
sem_t robot;  // Removed empty, full, mutex as they're not used

// Shared resources
int buffer_count = 0; // The counter replacing the array

// function to execute any machines 
void *machine(void *arg){
    Machine *m = (Machine *)arg;

    while(1) {
        sem_wait(&m->start);
        printf("Machine %d is processing ...\n", m->id);
        usleep(rand() % 1000001);
        printf("Machine %d ready, waiting robot...\n", m->id);
        sem_post(&m->done);
    }
}

// function to create all machines - FIXED return type
void create_machines(){  // Changed from void* to void
    for (int i = 0; i < NUM_MAQS; i++){
        machines[i].id = i + 1;
        sem_init(&machines[i].start, 0, 0);
        sem_init(&machines[i].done, 0, 0);

        pthread_create(&machine_threads[i], NULL, machine, &machines[i]);  // Store thread
    }
}

void *robo_mov(void *arg) {
    while (1) {
        sem_wait(&robot);
        
        // verify which machine done the processing
        int got_piece = 0;
        for(int i = 0; i < NUM_MAQS && !got_piece; i++) {
            if(sem_trywait(&machines[i].done) == 0) {
                printf("Robot retrieved piece from machine %d\n", machines[i].id);
                sem_post(&machines[i].start);
                got_piece = 1;
            }
        }
        
        if(got_piece) {  // Only access buffer if got a piece
            sem_wait(&B_slots);  // Using B_slots instead of empty
            sem_wait(&B_count);  // Using B_count instead of mutex
            buffer_count++;
            printf("Robot added piece to buffer. Items: %d\n", buffer_count);
            sem_post(&B_count);
            sem_post(&B_items);  // Using B_items instead of full
        }
        
        sem_post(&robot);
    }
}


void *retrive_from_buffer(void *arg) {
    while (1) {
        sem_wait(&B_items); // Wait for an item to be on buffer
        
        printf("[BUFFER] External agent preparing to remove piece...\n");
        usleep(rand() % 1000001);  // 0.5 a 1.5 segundos (mais realista)
        
        sem_wait(&B_count); // Lock counter
        if(buffer_count > 0) {  // Safety check
            buffer_count--;
            printf("[BUFFER] Piece removed from buffer. Current items: %d\n", buffer_count);
        }
        sem_post(&B_count); // Unlock counter
        
        sem_post(&B_slots); // Signal that a slot on the buffer is free
        
        printf("[BUFFER] External agent finished removal\n");
    }
}

int main() {
    srand(time(NULL));
    
    // Initialize semaphores
    sem_init(&robot, 0, 1);              // Robot initially free
    sem_init(&B_slots, 0, BUFFER_SIZE);  // BUFFER_SIZE free slots
    sem_init(&B_items, 0, 0);             // 0 items in buffer
    sem_init(&B_count, 0, 1);             // Mutex for counter
    
    printf("System initialized. Buffer capacity: %d\n", BUFFER_SIZE);
    
    // Create all machines
    create_machines();
    
    // Create robot and external agent threads
    pthread_t robo, external_agent;
    pthread_create(&robo, NULL, robo_mov, NULL);
    pthread_create(&external_agent, NULL, retrive_from_buffer, NULL);
    
    // Start machines with different intervals
    printf("\nStarting machines...\n");
    sleep(1);
    sem_post(&machines[0].start);  // Start machine 1
    sleep(2);
    sem_post(&machines[1].start);  // Start machine 2
    
    // Keep program running
    pthread_join(robo, NULL);
    pthread_join(external_agent, NULL);
    for(int i = 0; i < NUM_MAQS; i++) {
        pthread_join(machine_threads[i], NULL);
    }
    
    return 0;
}