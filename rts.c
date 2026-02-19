#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 2
#define NUM_MACH 2

typedef struct{
    int id;
    sem_t ready;
    sem_t empty;
} Machine;

Machine machines[NUM_MACH];
pthread_t machine_threads[NUM_MACH];
pthread_t robot_t, buffer_t;

sem_t robot;
sem_t B_items, B_slots;
sem_t B_count;

int buffer_count = 0;

void *feed_machine(void *arg) {
    Machine *m = (Machine *)arg;
    
    while(1) {
        sem_wait(&m->empty);
        printf("[MACHINE %d] Processing...\n", m->id);
        usleep(rand() % 1000001);
        printf("[MACHINE %d] Waiting robot...\n", m->id);
        sem_post(&m->ready);
    }
}

void *get_piece_from_machines(void *arg) {
    while (1) {
        sem_wait(&robot);
  
        int got_piece = 0;
        for(int i = 0; i < NUM_MACH && !got_piece; i++) {
            if(sem_trywait(&machines[i].ready) == 0) {
                printf("[ROBOT] Retrieved piece from Machine %d\n", machines[i].id);
                sem_post(&machines[i].empty);
                got_piece = 1;
            }
        }
        
        if(got_piece) {
            sem_wait(&B_slots);
            sem_wait(&B_count);
            buffer_count++;
            printf("[ROBOT] Added piece to buffer. Current Items: %d\n", buffer_count);
            sem_post(&B_count);
            sem_post(&B_items);
        }
        
        sem_post(&robot);
    }
}

void *retrive_from_buffer(void *arg) {
    while (1) {
        sem_wait(&B_items); // Wait for an item to be on buffer
        
        printf("[BUFFER] Waiting for external agent...\n");
        usleep(rand() % 1000001);
        
        sem_wait(&B_count); // Lock counter
        buffer_count--;
        printf("[BUFFER] Piece removed from buffer. Current items: %d\n", buffer_count);
        sem_post(&B_count); // Unlock counter
        
        sem_post(&B_slots); // Signal that a slot on the buffer is free
    }
}

void create_machines() {
    for (int i = 0; i < NUM_MACH; i++){
        machines[i].id = i;

        // Allow only 1 piece per machine
        sem_init(&machines[i].ready, 0, 0);
        sem_init(&machines[i].empty, 0, 1); 

        pthread_create(&machine_threads[i], NULL, feed_machine, &machines[i]);
    }
}

void create_robot() {
    sem_init(&robot, 0, 1);
    pthread_create(&robot_t, NULL, get_piece_from_machines, NULL);
}

void create_buffer() {
    sem_init(&B_items, 0, 0);  
    sem_init(&B_slots, 0, BUFFER_SIZE);
    sem_init(&B_count, 0, 1);
    pthread_create(&buffer_t, NULL, retrive_from_buffer, NULL);
}

void start_machines() {
    for(int i = 0; i < NUM_MACH; i++) {
        pthread_join(machine_threads[i], NULL);
    }
}

void start_robot() {
    pthread_join(robot_t, NULL);
}

void start_buffer() {
    pthread_join(buffer_t, NULL);
}

int main() {   
    srand(time(NULL));

    create_machines();
    create_robot();
    create_buffer();

    start_machines();
    start_robot();
    start_buffer();

    return 0;
}