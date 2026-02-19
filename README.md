# Simulação de uma Célula de Manufatura Automatizada

Este projeto apresenta a simulação de uma célula de manufatura industrial automatizada, desenvolvida em linguagem C, utilizando **Pthreads** e **Semáforos POSIX** para modelar concorrência, sincronização e compartilhamento de recursos. O sistema representa o funcionamento coordenado de máquinas de processamento, um robô industrial compartilhado e um buffer de saída com capacidade finita.

O objetivo principal da simulação é reproduzir os desafios típicos de sistemas industriais concorrentes, como a coordenação de tarefas paralelas, o controle de acesso a recursos compartilhados e a prevenção de situações indesejadas como bloqueios (deadlock) e violação de capacidade (overflow).

## Visão Geral do Sistema

A célula de manufatura é composta por três componentes principais, que operam de forma concorrente e sincronizada, formando um pipeline de produção:

1. **Máquinas de processamento:** As máquinas são unidades independentes responsáveis pelo processamento da matéria-prima. Cada máquina executa seu ciclo de trabalho de forma autônoma e, ao finalizar o processamento, entra em uma **fila de prioridade (FIFO)** e sinaliza ao robô que uma peça está pronta para ser retirada. A máquina permanece bloqueada até que o robô realize a coleta, garantindo a sequência lógica de produção.
2. **Robô Industrial:** O robô atua como o mestre de transporte. Ele aguarda sinais das máquinas e, seguindo a ordem de chegada na fila, identifica qual máquina deve atender. Em seguida, transporta a peça até o buffer de saída. Como é um recurso compartilhado, o robô atende uma solicitação por vez e implementa lógica para evitar esperas ocupadas.
3. **Buffer de saída:** Área de armazenamento temporário com capacidade limitada. As peças depositadas são removidas por um agente externo (consumidor), que atua em intervalos aleatórios, liberando espaço para novas peças e prevenindo o transbordamento do sistema.

## Códigos e uso

O código implementa a simulação utilizando **POSIX Threads** e semáforos. A organização é feita através de estruturas (`structs`) que agrupam os semáforos e variáveis de cada componente (Machine, Robot e Buffer).

### Pré-requisitos

* Ambiente Linux (ou WSL no Windows).
* Compilador `gcc`.
* Biblioteca `pthread`.

### Modelo 'rts.c'

Cada máquina possui dois semáforos: `ready` (sinaliza peça pronta) e `empty` (indica que a máquina está livre). O robô possui um semáforo `call`, que o retira do estado de bloqueio apenas quando há trabalho a ser feito, eliminando a **espera ocupada**.

Para garantir a justiça no atendimento, o robô utiliza uma **Fila Circular (FIFO)** protegida por um mutex. Quando uma máquina termina seu trabalho, ela insere seu ID no `tail` da fila. O robô, ao ser acordado, retira o ID do `head` da fila, garantindo que a máquina que terminou primeiro seja a primeira a ser atendida, evitando a "inanição" de máquinas mais lentas.

O buffer utiliza os semáforos `slots` (espaços vazios), `items` (peças prontas) e um `mutex` para proteger o contador `n_items`. O robô sempre executa um `sem_wait(buffer.slots)` antes de coletar uma peça, garantindo que nunca retirará uma peça da máquina se não houver onde colocá-la, prevenindo **deadlocks**.

#### Compilar e Executar

Abra seu terminal no diretório do projeto e execute:

```bash
gcc -o rts rts.c -lpthread && ./rts

```

#### Interpretando a saída

O programa registra eventos em tempo real no console:

* `[MACHINE X] Processing...`: Máquina está em operação.
* `[ROBOT] Retrieved piece from Machine X`: Robô coletou a peça seguindo a ordem da fila.
* `[BUFFER] Current items: X`: Status atual da ocupação do buffer.

Exemplo de log com a lógica de fila:

```
[MACHINE 0] Processing...
[MACHINE 1] Processing...
[MACHINE 0] Waiting robot...
[ROBOT] Retrieved piece from Machine 0
[ROBOT] Added piece to buffer. Current Items: 1
[MACHINE 0] Processing...
[BUFFER] Waiting for external agent...
[BUFFER] Piece removed from buffer. Current items: 0
[MACHINE 0] Waiting robot...
[ROBOT] Retrieved piece from Machine 0
[ROBOT] Added piece to buffer. Current Items: 1
[MACHINE 0] Processing...
[BUFFER] Waiting for external agent...
[MACHINE 1] Waiting robot...
[ROBOT] Retrieved piece from Machine 1
[ROBOT] Added piece to buffer. Current Items: 2
[MACHINE 1] Processing...
[MACHINE 0] Waiting robot...
[ROBOT] Retrieved piece from Machine 0
[MACHINE 0] Processing...
[MACHINE 0] Waiting robot...
[BUFFER] Piece removed from buffer. Current items: 1
[BUFFER] Waiting for external agent...
[ROBOT] Added piece to buffer. Current Items: 2
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

Acesse o link abaixo para entender o funcionamento do sistema: [https://www.youtube.com/watch?v=45e65I-3E7I](https://www.youtube.com/watch?v=45e65I-3E7I)

---