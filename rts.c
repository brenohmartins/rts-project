#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

#define BUFFER_SIZE 2

sem_t M1_ready, M1_empty; 
sem_t M2_ready, M2_empty;
sem_t B_items, B_slots;
sem_t B_count;

// Shared resources
int P1 = 0;
int P2 = 1;
int buffer_count = 0; // The counter replacing the array

void *feed_machine_1(void *arg) {
    while (1) {
        sem_wait(&M1_empty);
        printf("[MACHINE 1] Processing Piece %d...\n", P1);
        usleep(rand() % 1000001);
        printf("[MACHINE 1] Ready, waiting robot...\n");
        sem_post(&M1_ready); 
    }
}

void *feed_machine_2(void *arg) {
    while (1) {
        sem_wait(&M2_empty);
        printf("[MACHINE 2] Processing Piece %d...\n", P2);
        usleep(rand() % 1000001);
        printf("[MACHINE 2] Ready, waiting robot...\n");
        sem_post(&M2_ready); 
    }
}

void *get_piece_from_machines(void *arg) {
    int piece_on_robot = -1; 
    while (1){
        if (sem_trywait(&M1_ready) == 0 && piece_on_robot == -1) { // If machine has a piece
            sem_wait(&B_slots); // Wait for buffer to have a slot

            piece_on_robot = P1; // Get piece
            printf("[ROBOT] Retrieved Piece %d on Machine 1\n", P1);
            P1 += 2; // Next piece on machine will have this index
            sem_post(&M1_empty); // Machine is now free for a new piece

        } 
        else if (sem_trywait(&M2_ready) == 0 && piece_on_robot == -1) { // If machine has a piece
            sem_wait(&B_slots); // Wait for buffer to have a slot

            piece_on_robot = P2; // Get piece
            printf("[ROBOT] Retrieved Piece %d on Machine 2\n", P2);
            P2 += 2; // Next piece on machine will have this index
            sem_post(&M2_empty); // Machine is now free for a new piece

        } else {
            printf("[ROBOT] No pieces available! Waiting...\n");
        }

        if (piece_on_robot != -1) { // If robot has a piece (a slot will be already available)
            sem_wait(&B_count); // Lock counter
            buffer_count++;
            printf("[ROBOT] Added piece to buffer. Current items: %d\n", buffer_count);
            piece_on_robot = -1; // Robot is free
            sem_post(&B_count); // Unlock counter
            sem_post(&B_items); // Increase available items for consumer
        }
        usleep(100000); 
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

int main() {   
    srand(time(NULL));

    sem_init(&M1_ready, 0, 0); sem_init(&M1_empty, 0, 1); // Only allow 1 piece
    sem_init(&M2_ready, 0, 0); sem_init(&M2_empty, 0, 1); // Only allow 1 piece
    sem_init(&B_items, 0, 0);  sem_init(&B_slots, 0, BUFFER_SIZE); // Only allow BUFFER_SIZE pieces
    sem_init(&B_count, 0, 1); // Initialize counter mutex

    pthread_t machine_1_t, machine_2_t, robot_t, buffer_t;
    pthread_create(&machine_1_t, NULL, feed_machine_1, NULL);
    pthread_create(&machine_2_t, NULL, feed_machine_2, NULL);
    pthread_create(&robot_t, NULL, get_piece_from_machines, NULL);
    pthread_create(&buffer_t, NULL, retrive_from_buffer, NULL);

    pthread_join(machine_1_t, NULL);
    pthread_join(machine_2_t, NULL);
    pthread_join(robot_t, NULL);
    pthread_join(buffer_t, NULL);

    return 0;
}