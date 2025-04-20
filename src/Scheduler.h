#ifndef SCHEDULER_H
#define SCHEDULER_H

#include <vector>
#include <string>
#include "Process.h"

/** Holds the aggregated performance metrics resulting from a single CPU scheduling algorithm simulation run. */
struct SimulationResult {
    std::string algorithmName;
    double averageWaitingTime;
    double averageTurnaroundTime;
    double cpuUtilization;
    double throughput;
    double averageResponseTime;
    int totalElapsedTime;
    int totalProcessesCompleted;

    SimulationResult(std::string name = "") : 
        algorithmName(name),
        averageWaitingTime(0.0),
        averageTurnaroundTime(0.0),
        cpuUtilization(0.0),
        throughput(0.0),
        averageResponseTime(0.0),
        totalElapsedTime(0),
        totalProcessesCompleted(0) {}
};

/** Runs the First-Come, First-Served scheduling algorithm simulation. */
SimulationResult runFCFS(std::vector<Process> processes);

/** Runs the non-preemptive Shortest Job First scheduling algorithm simulation. */
SimulationResult runSJF(std::vector<Process> processes); 

/** Runs the preemptive Shortest Remaining Time First scheduling algorithm simulation. */
SimulationResult runSRTF(std::vector<Process> processes); 

/** Runs the Priority scheduling algorithm simulation (preemptive or non-preemptive). */
SimulationResult runPriority(std::vector<Process> processes, bool preemptive); 

/** Runs the Round Robin scheduling algorithm simulation with a specified time quantum. */
SimulationResult runRoundRobin(std::vector<Process> processes, int timeQuantum);

/** Runs the Multi-Level Feedback Queue scheduling algorithm simulation. */
SimulationResult runMLFQ(std::vector<Process> processes); 

/** Prints the detailed performance metrics for a single simulation result. */
void printResults(const SimulationResult& result);

/** Prints a formatted comparison table of performance metrics for multiple simulation results. */
void printComparison(const std::vector<SimulationResult>& results);


#endif 