# Simulação de uma Célula de Manufatura Automatizada

Este projeto apresenta a simulação de uma célula de manufatura industrial automatizada, desenvolvida em linguagem C, utilizando **Pthreads** e **Semáforos POSIX** para modelar concorrência, sincronização e compartilhamento de recursos. O sistema representa o funcionamento coordenado de duas máquinas de processamento, um robô industrial compartilhado e um buffer de saída com capacidade finita.

O objetivo principal da simulação é reproduzir os desafios típicos de sistemas industriais concorrentes, como a coordenação de tarefas paralelas, o controle de acesso a recursos compartilhados e a prevenção de situações indesejadas como bloqueios (deadlock) e violação de capacidade (overflow).

## Visão Geral do Sistema

A célula de manufatura é composta por três componentes principais, que operam de forma concorrente e sincronizada, formando um pipeline de produção:

1. **Máquinas de processamento (M1 & M2):** As máquinas são unidades independentes responsáveis pelo processamento da matéria-prima. Cada máquina executa seu ciclo de trabalho de forma autônoma e, ao finalizar o processamento, sinaliza ao robô que uma preça está pronta para ser retirada. Até o processo ser finalizado, a máquina permanece bloqueada até que o robô realize a coleta da peça, garantindo que um novo ciclo de produção só seja iniciado após a liberação adequada da peça.
2. **Robô Industrial:** O robô monitora continuamente o estado das máquinas, identifica quando uma peça está disponível e realiza a retirada desta. Em seguida, ele transporta a peça até o buffer de saída. Como é um recurso compartilhado, o robô pode atender apenas uma máquina por vez e possui capacidade unitária de transporte.
3. **Buffer de saída:** Representa uma esteira ou área de armazenamento temporário destinada às peças finalizadas. Esse buffer possui capacidade limitada, definida como dois slots, de modo a evidenciar possíveis problemas de saturação do sistema. As peças depositadas no buffer são removidas por um agente externo, modelado como um processo independente que atua em intervalos de tempo aleatórios, liberando espaço para novas peças.

## Códigos e uso

O código implementa a simulação concorrente de uma célula de manufatura automatizada utilizando linguagem C, com POSIX Threads (pthreads) e semáforos POSIX (sem_t). O sistema é composto por duas máquinas produtoras, um robô responsável pelo transporte das peças, um buffer de saída com capacidade limitada e um agente externo que realiza a remoção das peças armazenadas. A sincronização entre esses componentes é feita por meio de semáforos.

### Pré-requisitos

* Ambiente Linux (ou WSL no Windows).
* Compilador `gcc` .
* Biblioteca `pthread` (padrão na maioria das distribuições Linux).


### Modelo 'rts.c'

Cada máquina possui dois semáforos: um que indica se está livre para iniciar o processamento (M1_empty e M2_empty) e outro que sinaliza que a peça foi finalizada (M1_ready e M2_ready). Desse modo, cada máquina produz apenas uma peça por vez, aguardando o robô realizar a coleta antes de iniciar um novo ciclo.

O buffer possui capacidade limitada definida pela constante 'BUFFER_SIZE', com valor igual a 2. Para o controle dele, são utilizados três semáforos: 'B_slots', 'B_items' e 'B_count'. O semáforo 'B_slots' é inicializado com o valor correspondente à capacidade do buffer e controla a quantidade de espaços livres disponíveis. O semáforo 'B_items' é inicializado com valor 0 e representa a quantidade de peças prontas para remoção. Já o semáforo  'B_count' atua como um mutex, sendo inicializado com valor 1 e utilizado para proteger a variável compartilhada buffer_count, que armazena a quantidade atual de itens no buffer. As variáveis P1 e P2 identificam as peças produzidas por cada máquina, sendo incrementadas de dois em dois para diferenciar a origem das peças. A variável buffer_count representa a quantidade atual de itens armazenados.

As máquinas executam em laço contínuo, simulando o processamento com tempo aleatório e sinalizando quando a peça está pronta. Em seguida, permanecem bloqueadas até que o robô libere novamente o semáforo de disponibilidade. O robô monitora ambas as máquinas utilizando sem_trywait, verificando se há peças prontas sem bloquear sua execução. Ao encontrar uma peça, ele primeiro garante que exista espaço no buffer por meio de sem_wait(B_slots), evitando overflow. Em seguida, coleta a peça, libera a máquina correspondente, incrementa o contador do buffer dentro de uma região crítica protegida por B_count e sinaliza a disponibilidade do item com sem_post(B_items).

O agente externo aguarda a existência de itens no buffer por meio de sem_wait(B_items). Quando há peça disponível, simula o tempo de remoção, decrementa buffer_count em região protegida e libera um espaço no buffer com sem_post(B_slots). Assim, essa aplicação garante uma sincronização adequada entre robô, máquinas e buffer de saída. 


#### Compilar e Executar

Abra seu terminal no diretório do projeto e execute:

```bash
gcc -o rts rts.c -lpthread && ./rts
```

#### Interpretando a saída

O programa registra eventos em tempo real no console:

* `[MACHINE X] Processing...`: Máquina está cheia
* `[ROBOT] Retrieved...`: Robô moveu a peça.
* `[BUFFER] Current items: X`: Status em tempo real do armazenamento de saída.
  
Esse é um exemplo obtido ao rodar o código:
```
[MACHINE 2] Processing Piece 1...
[ROBOT] No pieces available! Waiting...
[MACHINE 1] Processing Piece 0...
[ROBOT] No pieces available! Waiting...
[MACHINE 1] Ready, waiting robot...
[ROBOT] Retrieved Piece 0 on Machine 1
[ROBOT] Added piece to buffer. Current items: 1
[MACHINE 1] Processing Piece 2...
[BUFFER] Waiting for external agent...
[ROBOT] No pieces available! Waiting...
[BUFFER] Piece removed from buffer. Current items: 0
[ROBOT] No pieces available! Waiting...
[MACHINE 1] Ready, waiting robot...
[MACHINE 2] Ready, waiting robot...
[ROBOT] Retrieved Piece 2 on Machine 1
[ROBOT] Added piece to buffer. Current items: 1
[BUFFER] Waiting for external agent...
[MACHINE 1] Processing Piece 4...
[ROBOT] Retrieved Piece 1 on Machine 2
[ROBOT] Added piece to buffer. Current items: 2
[MACHINE 2] Processing Piece 3...
[BUFFER] Piece removed from buffer. Current items: 1
[BUFFER] Waiting for external agent...
[ROBOT] No pieces available! Waiting...

```
#### Diagramas de estados para os componentes do sistema

Abaixo, estão os diagramas de estados referentes aos componentes do sistema: as máquinas, o robô industrial e o buffer de saída, respectivamente.

<div align = "center">
<img src = "https://github.com/user-attachments/assets/93351a61-8021-4028-b09c-41075f68e33c" width = "500px" />
</div>

<div align = "center">
<img src = "https://github.com/user-attachments/assets/9a5f3699-ea2d-4b2d-9963-7c8ac2406e72" width = "500px" />
</div>

<div align = "center">
<img src = "https://github.com/user-attachments/assets/5582ef57-cf72-4f67-91f7-67b529f4b586" width = "500px" />
</div>

#### Vídeo Explicativo

Acesse o link abaixo para entender o funcionamento do sistema: https://www.youtube.com/watch?v=45e65I-3E7I

---
