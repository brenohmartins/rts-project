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
    sem_t ready, empty;
    pthread_t thread;
} Machine;
Machine machines[NUM_MACH];

typedef struct{
    int n_items;
    pthread_t thread;
    sem_t items, slots, count;
} Buffer;
Buffer buffer;

typedef struct{
    sem_t ready;
    pthread_t thread;
} Robot;
Robot robot; 

void *feed_machine(void *arg) {
    Machine *machine = (Machine *)arg;
    
    while(1) {
        sem_wait(&machine->empty);
        printf("[MACHINE %d] Processing...\n", machine->id);
        usleep(rand() % 1000001);
        printf("[MACHINE %d] Waiting robot...\n", machine->id);
        sem_post(&machine->ready);
    }
}

void *get_piece_from_machines(void *arg) {
    while (1) {
        sem_wait(&robot.ready);
  
        int got_piece = 0;
        for(int i = 0; i < NUM_MACH && !got_piece; i++) {
            if(sem_trywait(&machines[i].ready) == 0) {
                printf("[ROBOT] Retrieved piece from Machine %d\n", machines[i].id);
                sem_post(&machines[i].empty);
                got_piece = 1;
            }
        }
        
        if(got_piece) {
            sem_wait(&buffer.slots);
            sem_wait(&buffer.count);
            buffer.n_items++;
            printf("[ROBOT] Added piece to buffer. Current Items: %d\n", buffer.n_items);
            sem_post(&buffer.count);
            sem_post(&buffer.items);
        }
        
        sem_post(&robot.ready);
    }
}

void *retrive_from_buffer(void *arg) {
    while (1) {
        sem_wait(&buffer.items); // Wait for an item to be on buffer
        
        printf("[BUFFER] Waiting for external agent...\n");
        usleep(rand() % 1000001);
        
        sem_wait(&buffer.count); // Lock counter
        buffer.n_items--;
        printf("[BUFFER] Piece removed from buffer. Current items: %d\n", buffer.n_items);
        sem_post(&buffer.count); // Unlock counter
        
        sem_post(&buffer.slots); // Signal that a slot on the buffer is free
    }
}

void create_machines() {
    for (int i = 0; i < NUM_MACH; i++){
        machines[i].id = i;

        sem_init(&machines[i].ready, 0, 0);
        sem_init(&machines[i].empty, 0, 1); 

        pthread_create(&machines[i].thread, NULL, feed_machine, &machines[i]);
    }
}

void create_robot() {
    sem_init(&robot.ready, 0, 1);
    pthread_create(&robot.thread, NULL, get_piece_from_machines, NULL);
}

void create_buffer() {
    sem_init(&buffer.items, 0, 0);  
    sem_init(&buffer.slots, 0, BUFFER_SIZE);
    sem_init(&buffer.count, 0, 1);
    pthread_create(&buffer.thread, NULL, retrive_from_buffer, NULL);
}

void start_machines() {
    for(int i = 0; i < NUM_MACH; i++) {
        pthread_join(machines[i].thread, NULL);
    }
}

void start_robot() {
    pthread_join(robot.thread, NULL);
}

void start_buffer() {
    pthread_join(buffer.thread, NULL);
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