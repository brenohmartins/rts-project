#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// ordem de execução:  Máquina → Robô → Buffer → Remoção externa

#define BUFFER_SIZE = 2  // capacidade de itens no buffer(esteira)

sem_t m1_start, m2_start;     // comando de início das máquinas 1 e 2
sem_t m1_done, m2_done;       // peça pronta para ser pega pelo robô móvel
sem_t robot;                  // robô móvel com capacidade de carga de 1 item
sem_t vazio, cheio, mutex;      // espaços livres no buffer, peças no buffer, exclusão mútua de acesso ao buffer

void *maquina1(void *arg) {
    while (1) {
        sem_wait(&m1_start);  // espera a máquina 1 ser inicializada

        printf("Máquina 1 está processando a peça...\n");

        sleep(rand() % 5 + 8);  // Tempo de produção da máquina entre 8 a 12 segundos

        printf("Peça na Máquina 1 está pronta, aguardando robô...\n");

        sem_post(&m1_done);    //máquina finalizou a produção e o item está pronto para ser pego pelo robô móvel
    }
}

void *maquina2(void *arg) {
    while (1) {
        sem_wait(&m2_start);

        printf("Máquina 2 está processando a peça...\n");

        sleep(rand() % 5 + 8);  // Tempo de produção da máquina entre 8 a 12 segundos

        printf("Peça na Máquina 2 está pronta, aguardando robô...\n");

        sem_post(&m2_done);    //máquina finalizou a produção e o item está pronto para ser pego pelo robô móvel
    }
}

void *robo_mov(void *arg) {
    while (1){
        sem_wait(&robot);  // espera o robô móvel estar livre de carga

        if sem_tyrwait(&m1_done) == 0; {     //analise se a máquina 1 finalizou a produção e o item pode ser pego pelo robô móvel
            printf("O robô móvel pegou peça da Máquina 1\n");
            sem_post(&m1_start);          // com a retirada do item da máquina 1, ela volta ao estado de inicialização
        }

        else {
            sem_wait(&m2_done);
            printf("O robô móvel pegou a peça da Máquina 2\n");
            sem_post(&m2_start);
        }

        sem_wait(&vazio);   // analisa se existe espaço no buffer(esteira) para que o robô móvel entregue o item tradizo da máquina 1 ou 2
        sem_wait(&mutex);

        printf("O robô móvel depositou peça na esteira\n");

        sem_post(&mutex);  // atualiza estado do buffer
        sem_post(&cheio);  // indica  que há peça disponível

        sem_post(&robot);  // robô móvel volta ao estado livre

    }
}

void *remov_buff(void *arg) {

    while (1) {
        sleep(rand()% 2 + 3); // tempo de retirada do item da esteira, (ação externa aleatória) tempo de ação de 3 a 5 segundos

        sem_wai(&cheio);      // analisa se tem algum item dentro do buffer (esteira)

        sem_wait(&mutex);    // acessa o buffer

        printf("Item removido da esteira\n");

        sem_post(&mutex);    // sai do buffer

        sem_post(&vazio);   // libera o espaço do item no buffer(esteira)
    }
}

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <semaphore.h>
#include <unistd.h>
#include <time.h>

// ordem de execução:  Máquina → Robô → Buffer → Remoção externa

#define BUFFER_SIZE 2  // capacidade de itens no buffer(esteira)

sem_t m1_start, m2_start;     // comando de início das máquinas 1 e 2
sem_t m1_done, m2_done;       // peça pronta para ser pega pelo robô móvel
sem_t robot;                  // robô móvel com capacidade de carga de 1 item
sem_t vazio, cheio, mutex;     // espaços livres no buffer, peças no buffer, exclusão mútua de acesso ao buffer

void *maquina1(void *arg) {
    while (1) {
        sem_wait(&m1_start);  // espera a máquina 1 ser inicializada

        printf("Máquina 1 está processando a peça...\n");

        sleep(rand() % 5 + 8);  // Tempo de produção da máquina entre 8 a 12 segundos

        printf("Peça na Máquina 1 está pronta, aguardando robô...\n");

        sem_post(&m1_done);    //máquina finalizou a produção e o item está pronto para ser pego pelo robô móvel
    }
}

void *maquina2(void *arg) {
    while (1) {
        sem_wait(&m2_start);

        printf("Máquina 2 está processando a peça...\n");

        sleep(rand() % 5 + 8);  // Tempo de produção da máquina entre 8 a 12 segundos

        printf("Peça na Máquina 2 está pronta, aguardando robô...\n");

        sem_post(&m2_done);    //máquina finalizou a produção e o item está pronto para ser pego pelo robô móvel
    }
}

void *robo_mov(void *arg) {
    while (1){
        sem_wait(&robot);  // espera o robô móvel estar livre de carga

        if (sem_trywait(&m1_done) == 0) {     //analise se a máquina 1 finalizou a produção e o item pode ser pego pelo robô móvel
            sleep(rand() % 2 + 3);  // Tempo do robô ir até a máquina pegar o item, de 3 a 5 segundos
            printf("O robô móvel pegou peça da Máquina 1\n");
            sem_post(&m1_start);          // com a retirada do item da máquina 1, ela volta ao estado de inicialização
        }

        else {
            sem_wait(&m2_done);
            sleep(rand() % 2 + 3);  // Tempo do robô ir até a máquina pegar o item, de 3 a 5 segundos
            printf("O robô móvel pegou a peça da Máquina 2\n");
            sem_post(&m2_start);
        }

        sem_wait(&vazio);   // analisa se existe espaço no buffer(esteira) para que o robô móvel entregue o item tradizo da máquina 1 ou 2
        sem_wait(&mutex);

        sleep(rand() % 3 + 5);  // Tempo do robô ir da máquina até a esteira 5 a 8 segundos

        printf("O robô móvel depositou peça na esteira\n");

        sem_post(&mutex);  // atualiza estado do buffer
        sem_post(&cheio);  // indica  que há peça disponível

        sem_post(&robot);  // robô móvel volta ao estado livre

    }
}

void *remov_buff(void *arg) {

    while (1) {
    
        sem_wait(&cheio);      // analisa se tem algum item dentro do buffer (esteira)

        sem_wait(&mutex);    // acessa o buffer

        sleep(rand()% 5 + 10); // tempo de retirada do item da esteira, (ação externa aleatória) tempo de ação de 10 a 15 segundos

        printf("Item removido da esteira\n");

        sem_post(&mutex);    // sai do buffer

        sem_post(&vazio);   // libera o espaço do item no buffer(esteira)
    }
}

int main () {
    srand(time(NULL));

    //buffer
    sem_init(&mutex, 0, 1);
    sem_init(&cheio, 0, 0);
    sem_init(&vazio, 0, BUFFER_SIZE);

    //maquinas
    sem_init(&m1_start, 0, 1);
    sem_init(&m2_start, 0, 1);
    
    //robo
    sem_init(&robot, 0, 1);

    //cria as threads das maquinas 1 e 2, robô móvel e da esteira
    pthread_t t_maq1, t_maq2, t_robo, t_buffer;

    pthread_create(& t_maq1, NULL, maquina1, NULL);
    pthread_create(& t_maq2, NULL, maquina2, NULL);
    pthread_create(& t_robo, NULL, robo_mov, NULL);
    pthread_create(& t_buffer, NULL, remov_buff, NULL);

    
    pthread_join(t_maq1, NULL);
    pthread_join(t_maq2, NULL);
    pthread_join(t_robo, NULL);
    pthread_join(t_buffer, NULL);

    return 0;
    
}
