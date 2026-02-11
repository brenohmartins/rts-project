# Simulação de uma Célula de Manufatura Automatizada

Este projeto apresenta a simulação de uma célula de manufatura industrial automatizada, desenvolvida em linguagem C, utilizando **Pthreads** e **Semáforos POSIX** para modelar concorrência, sincronização e compartilhamento de recursos. O sistema representa o funcionamento coordenado de duas máquinas de processamento, um robô industrial compartilhado e um buffer de saída com capacidade finita.

O objetivo principal da simulação é reproduzir os desafios típicos de sistemas industriais concorrentes, como a coordenação de tarefas paralelas, o controle de acesso a recursos compartilhados e a prevenção de situações indesejadas como bloqueios (deadlock) e violação de capacidade (overflow).

## Visão Geral do Sistema

A célula de manufatura é composta por três componentes principais, que operam de forma concorrente e sincronizada, formando um pipeline de produção:

1. **Máquinas de processamento (M1 & M2):** As máquinas são unidades independentes responsáveis pelo processamento da matéria-prima. Cada máquina executa seu ciclo de trabalho de forma autônoma e, ao finalizar o processamento, sinaliza ao robô que uma preça está pronta para ser retirada. Até o processo ser finalizado, a máquina permanece bloqueada até que o robô realize a coleta da peça, garantindo que um novo ciclo de produção só seja iniciado após a liberação adequada da peça.
2. **Robô Industrial:** O robô monitora continuamente o estado das máquinas, identifica quando uma peça está disponível e realiza a retirada desta. Em seguida, ele transporta a peça até o buffer de saída. Como é um recurso compartilhado, o robô pode atender apenas uma máquina por vez e possui capacidade unitária de transporte.
3. **Buffer de saída:** Representa uma esteira ou área de armazenamento temporário destinada às peças finalizadas. Esse buffer possui capacidade limitada, definida como dois slots, de modo a evidenciar possíveis problemas de saturação do sistema. As peças depositadas no buffer são removidas por um agente externo, modelado como um processo independente que atua em intervalos de tempo aleatórios, liberando espaço para novas peças.

## 🚀 Usage

### Prerequisites

* A Linux environment (or WSL on Windows).
* `gcc` compiler.
* `pthread` library (standard on most Linux distributions).

### Compile and Run

Open your terminal in the project directory and run:

```bash
gcc -o rts rts.c -lpthread && ./rts
```

### Interpreting the Output

The program logs real-time events to the console:

* `[MACHINE X] Processing...`: Machine is busy.
* `[ROBOT] Retrieved...`: Robot has moved a piece.
* `[BUFFER] Current items: X`: Real-time status of the output storage.

This is an example obtained by running the code:
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

---
