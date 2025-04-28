# CS3502 Project 2: CPU Scheduling Simulation

## Repository Structure

```
/
├── src/
│   ├── main.cpp
│   ├── Scheduler.cpp
│   ├── Process.h
│   ├── Scheduler.h
│   └── test/
│       ├── basic_test.txt
│       ├── arrival0.txt
│       ├── burst_mix.txt
│       └── large_scale.txt
├── report/
│   ├── report.tex
│   └── references.bib
└── README.md
```

## Overview

This project implements a C++ console application to simulate and evaluate various CPU scheduling algorithms. It aims to explore fundamental operating system concepts related to process scheduling, performance metrics, and algorithm comparison.

The simulator reads process definitions from text files, executes different scheduling policies, calculates key performance metrics, and presents a comparative analysis.

## Features

*   **Multiple Scheduling Algorithms:** Implements First-Come, First-Served (FCFS), Shortest Job First (SJF - Non-Preemptive), Shortest Remaining Time First (SRTF - Preemptive SJF), Priority (Non-Preemptive and Preemptive), Round Robin (RR), and Multi-Level Feedback Queue (MLFQ).
*   **Process Representation:** Uses a `Process` struct to hold process ID, arrival time, burst time, priority, and state variables for metric calculation.
*   **Input File Parsing:** Reads process data from comma-separated value (CSV) text files (format: `ID,ArrivalTime,BurstTime,Priority`). Handles comments (`#`) and empty lines.
*   **Performance Metrics:** Calculates Average Waiting Time (AWT), Average Turnaround Time (ATT), Average Response Time (ART), CPU Utilization (%), and Throughput (processes per time unit).
*   **Comparative Output:** Displays a formatted table comparing the calculated metrics for all implemented algorithms for a given input file.

## Components

*   **`main.cpp`:** Handles command-line arguments (input filename), orchestrates file loading (`loadProcessesFromFile`), runs each scheduling algorithm simulation, collects results, and calls the comparison printing function (`printComparison`).
*   **`Scheduler.cpp`:** Contains the core simulation logic for each scheduling algorithm (`runFCFS`, `runSJF`, `runSRTF`, `runPriority`, `runRoundRobin`, `runMLFQ`). Also includes the `calculateMetrics` function.
*   **`Process.h`:** Defines the `Process` struct, which encapsulates all necessary attributes and state for a process within the simulation.
*   **`Scheduler.h`:** Contains function prototypes and necessary includes for `Scheduler.cpp`.
*   **Input Files (`*.txt`):** Text files defining the processes for different test scenarios.

## Dependencies

This project is designed to be compiled and run in a Linux-like environment, such as Ubuntu running on the Windows Subsystem for Linux (WSL).

*   **WSL & Ubuntu:** If using Windows, enable WSL and install Ubuntu:
    ```bash
    wsl --install
    # Then install Ubuntu from the Microsoft Store
    ```
*   **GCC Compiler:** A modern C++ compiler supporting C++11 or later is required. Install the `build-essential` package on Ubuntu, which includes `g++`:
    ```bash
    sudo apt update
    sudo apt install build-essential
    g++ --version
    ```

### C++ Headers Used

The project utilizes standard C++ libraries. Key headers include:

*   `<iostream>`: For console input/output (printing results, error messages).
*   `<fstream>`: For reading the process definition files.
*   `<vector>`: For storing collections of processes and simulation results.
*   `<string>`: For handling filenames and processing text lines.
*   `<sstream>`: For parsing comma-separated values from input lines.
*   `<stdexcept>`: For throwing and catching runtime errors (e.g., file open, parsing issues).
*   `<numeric>`: Used for accumulating sums (e.g., in metric calculations).
*   `<algorithm>`: For sorting processes and finding elements (e.g., min element in SJF/SRTF).
*   `<iomanip>`: For formatting the output comparison table (`std::setw`, `std::setprecision`).
*   `<queue>`: Used as a ready queue data structure (e.g., in RR, MLFQ).
*   `<limits>`: Used for getting maximum integer values (e.g., initializing minimums in SRTF).
*   `<list>`: Used as a ready queue/list data structure (e.g., in FCFS, SJF, SRTF).

These headers provide necessary functions and classes for file I/O, data structures, algorithms, string manipulation, and formatted output.

## Compilation and Execution

Compile the project using the `g++` compiler in your Linux/Ubuntu terminal. Navigate to the **root directory** of the project.

**Compilation:**

```bash
g++ src/main.cpp src/Scheduler.cpp -o scheduler
```
*   `-o scheduler`: Specifies the output executable name as `scheduler` (created in the root directory).
*   `src/main.cpp src/Scheduler.cpp`: Lists the source files to compile, referencing their location within `src`.

**Execution:**

Run the compiled executable (from the root directory), providing the path to a process input file within the `src/test/` directory:

```bash
./scheduler src/test/<input_file.txt>
```

**Examples:**

```bash
./scheduler src/test/basic_test.txt
./scheduler src/test/arrival0.txt
./scheduler src/test/burst_mix.txt
./scheduler src/test/large_scale.txt
```

## Sample Output

The simulator outputs a table comparing the performance metrics for all implemented algorithms based on the provided input file.

**Output for `src/test/basic_test.txt`:**

```
Successfully loaded 5 processes from src/test/basic_test.txt

--- Performance Comparison ---
Algorithm                                    Avg Waiting    Avg Turnaround Avg Response   CPU Util (%)   Throughput (proc/unit)
----------------------------------------------------------------------------------------------------------------------------------
FCFS                                         11.40          17.00          11.40          100.00         0.18
SJF (Non-Preemptive)                         8.20           13.80          8.20           100.00         0.18
SRTF (Preemptive SJF)                        6.60           12.20          4.40           100.00         0.18
Priority (Non-Preemptive)                    10.60          16.20          10.60          100.00         0.18
Priority (Preemptive)                        9.60           15.20          7.80           100.00         0.18
Round Robin (Quantum=4)                      13.00          18.60          6.00           100.00         0.18
MLFQ (Q0:RR8, Q1:RR16, Q2:FCFS, Age:50)      12.40          18.00          11.00          100.00         0.18
----------------------------
```

**Output for `src/test/arrival0.txt`:**

```
Successfully loaded 5 processes from src/test/arrival0.txt

--- Performance Comparison ---
Algorithm                                    Avg Waiting    Avg Turnaround Avg Response   CPU Util (%)   Throughput (proc/unit)
----------------------------------------------------------------------------------------------------------------------------------
FCFS                                         13.40          19.00          13.40          100.00         0.18
SJF (Non-Preemptive)                         7.60           13.20          7.60           100.00         0.18
SRTF (Preemptive SJF)                        7.60           13.20          7.60           100.00         0.18
Priority (Non-Preemptive)                    11.20          16.80          11.20          100.00         0.18
Priority (Preemptive)                        11.20          16.80          11.20          100.00         0.18
Round Robin (Quantum=4)                      15.00          20.60          8.00           100.00         0.18
MLFQ (Q0:RR8, Q1:RR16, Q2:FCFS, Age:50)      14.40          20.00          13.00          100.00         0.18
----------------------------
```

**Output for `src/test/burst_mix.txt`:**

```
Successfully loaded 5 processes from src/test/burst_mix.txt

--- Performance Comparison ---
Algorithm                                    Avg Waiting    Avg Turnaround Avg Response   CPU Util (%)   Throughput (proc/unit)
----------------------------------------------------------------------------------------------------------------------------------
FCFS                                         18.60          28.80          18.60          100.00         0.10
SJF (Non-Preemptive)                         14.00          24.20          14.00          100.00         0.10
SRTF (Preemptive SJF)                        5.20           15.40          4.00           100.00         0.10
Priority (Non-Preemptive)                    23.40          33.60          23.40          100.00         0.10
Priority (Preemptive)                        22.00          32.20          16.60          100.00         0.10
Round Robin (Quantum=4)                      9.80           20.00          3.20           100.00         0.10
MLFQ (Q0:RR8, Q1:RR16, Q2:FCFS, Age:50)      11.40          21.60          5.60           100.00         0.10
----------------------------
```

**Output for `src/test/large_scale.txt`:**

```
Successfully loaded 25 processes from src/test/large_scale.txt

--- Performance Comparison ---
Algorithm                                    Avg Waiting    Avg Turnaround Avg Response   CPU Util (%)   Throughput (proc/unit)
----------------------------------------------------------------------------------------------------------------------------------
FCFS                                         112.80         124.36         112.80         100.00         0.09
SJF (Non-Preemptive)                         75.20          86.76          75.20          100.00         0.09
SRTF (Preemptive SJF)                        74.40          85.96          71.40          100.00         0.09
Priority (Non-Preemptive)                    100.12         111.68         100.12         100.00         0.09
Priority (Preemptive)                        104.68         116.24         89.12          100.00         0.09
Round Robin (Quantum=4)                      153.36         164.92         40.52          100.00         0.09
MLFQ (Q0:RR8, Q1:RR16, Q2:FCFS, Age:50)      150.12         161.68         59.60          100.00         0.09
----------------------------
```
