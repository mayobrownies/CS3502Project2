#ifndef PROCESS_H
#define PROCESS_H

#include <vector>

/**
 * Represents a single process in the CPU scheduling simulation.
 * Stores essential process characteristics like ID, arrival time, burst time,
 * and priority, along with state variables and metrics tracked during simulation 
 * (e.g., start time, completion time, remaining time, waiting time, etc.).
 */
struct Process {
    int id;
    int arrivalTime;
    int burstTime;
    int priority;

    int startTime = -1;
    int completionTime = -1;
    int waitingTime = 0;
    int turnaroundTime = 0;
    int remainingBurstTime;
    int responseTime = -1;

    /** Constructs a Process object. */
    Process(int pid, int arrival, int burst, int prio)
        : id(pid), arrivalTime(arrival), burstTime(burst), priority(prio), remainingBurstTime(burst) {}
};

#endif 