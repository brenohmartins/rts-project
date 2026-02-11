# Automated Manufacturing Cell Simulation

A multi-threaded C simulation of an industrial manufacturing cell using **POSIX Semaphores** and **Pthreads**. The project models two machines, a shared transport robot, and a finite buffer.

## 🛠 System Overview

The system consists of three main components operating in a synchronized pipeline:

1. **Machines (M1 & M2):** Independent units that process raw materials. They signal the robot when a piece is ready and wait for it to be collected before starting the next cycle.
2. **Industrial Robot:** It monitors both machines, retrieves finished pieces, and transports them to the output buffer.
3. **Output Buffer (Conveyor):** A storage area with a fixed capacity (default: 2 slots). An external agent removes pieces from this buffer at random intervals.

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

---