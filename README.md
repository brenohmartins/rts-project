# Simulação de uma Célula de Manufatura Automatizada

Este projeto apresenta a simulação de uma célula de manufatura industrial automatizada, desenvolvida em linguagem C, utilizando **Pthreads** e **Semáforos POSIX** para modelar concorrência, sincronização e compartilhamento de recursos. O sistema representa o funcionamento coordenado de duas máquinas de processamento, um robô industrial compartilhado e um buffer de saída com capacidade finita.

O objetivo principal da simulação é reproduzir os desafios típicos de sistemas industriais concorrentes, como a coordenação de tarefas paralelas, o controle de acesso a recursos compartilhados e a prevenção de situações indesejadas como bloqueios (deadlock) e violação de capacidade (overflow).

## Visão Geral do Sistema

A célula de manufatura é composta por três componentes principais, que operam de forma concorrente e sincronizada, formando um pipeline de produção:

1. **Máquinas de processamento (M1 & M2):** As máquinas são unidades independentes responsáveis pelo processamento da matéria-prima. Cada máquina executa seu ciclo de trabalho de forma autônoma e, ao finalizar o processamento, sinaliza ao robô que uma preça está pronta para ser retirada. Até o processo ser finalizado, a máquina permanece bloqueada até que o robô realize a coleta da peça, garantindo que um novo ciclo de produção só seja iniciado após a liberação adequada da peça.
2. **Robô Industrial:** O robô monitora continuamente o estado das máquinas, identifica quando uma peça está disponível e realiza a retirada desta. Em seguida, ele transporta a peça até o buffer de saída. Como é um recurso compartilhado, o robô pode atender apenas uma máquina por vez e possui capacidade unitária de transporte.
3. **Buffer de saída:** Representa uma esteira ou área de armazenamento temporário destinada às peças finalizadas. Esse buffer possui capacidade limitada, definida como dois slots, de modo a evidenciar possíveis problemas de saturação do sistema. As peças depositadas no buffer são removidas por um agente externo, modelado como um processo independente que atua em intervalos de tempo aleatórios, liberando espaço para novas peças.

## Códigos e uso

Para representar o funcionamento da célula de manufatura automatizada, foram desenvolvidas duas implementações distintas em linguagem C, ambas baseadas em programação concorrente com Pthreads e Semáforos POSIX. Embora os dois códigos modelam o mesmo sistema físico, cada implementação adota uma estratégia de sincronização diferente.

### Pré - requisitos

* Ambiente Linux (ou WSL no Windows).
* Compilador `gcc` .
* Biblioteca `pthread` (padrão na maioria das distribuições Linux).


### Modelo 1 - 'script.c'

O primeiro código implementa a célula de manufatura utilizando um modelo fortemente baseado em eventos e sinalizações diretas entre os componentes do sistema. Cada elemento físico (máquinas, robô e buffer) é representado por uma thread independente, e a comunicação entre elas é realizada por meio de semáforos que modelam estados e eventos do sistema. A ordem lógica de operação segue o fluxo: Máquina -> Robô -> Buffer -> Remoção Externa.

As máquinas M1 e M2 são representadas pelas funções 'maquina1' e 'maquina2'. Cada máquina opera em um laço infinito, alternando entre os estados de processamento e espera pela retirada da peça. O semáforo 'mx_start' representa a autorização para o início de um novo ciclo de produção, já o término do processamento é sinalizado por meio do semáforo mx_done, indicando que ela pode ser coletada pelo robô. Por fim, a máquina fica impossibilitada de iniciar um novo ciclo até que o robô retire a peça.

O robô é representado pela função 'robo_mov' e modela o robô de capacidade unitária, controlada pelo semáforo 'robot'. O comportamento do robô segue os passos: Aguarda estar livre para transporte -> Verifica se alguma máquina possui peça pronta ('mx_done'), retira a peça da máquina, aguarda a existencia de espaço livre no buffer ('vazio'), deposita a peça no buffer e retorna ao estado livre.

Por fim, para modelagem do buffer de saída, são utilizados os semáforos 'vazio', 'cheio' e 'mutex', que representam o número de slots disponíveis no buffer. A função 'remove_buff' representa um agente externo que remove as peças em intervalos aleatórios.

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

### Modelo 2 - 'rts.c'

---
