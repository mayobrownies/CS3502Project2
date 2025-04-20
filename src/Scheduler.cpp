#include "Scheduler.h"
#include <iostream>
#include <vector>
#include <numeric>
#include <algorithm>
#include <iomanip> 
#include <queue>   
#include <limits> 
#include <list> 

/**
 * Calculates final performance metrics after a simulation run.
 * Computes average waiting time, average turnaround time, average response time, CPU utilization, and throughput based on the state of completed processes and the total simulation time.
 * Updates the passed SimulationResult structure with these calculated metrics.
 */
void calculateMetrics(SimulationResult& result, std::vector<Process>& completedProcesses, int currentTime, int totalIdleTime) {
    if (completedProcesses.empty()) {
        result.totalProcessesCompleted = 0;
        result.totalElapsedTime = currentTime;
        return; 
    }

    double totalWaitingTime = 0;
    double totalTurnaroundTime = 0;
    double totalResponseTime = 0;
    int validResponseTimes = 0;

    // Ensure completed processes are sorted by ID for consistent metric calculation viewing (optional)
    std::sort(completedProcesses.begin(), completedProcesses.end(), [](const Process& a, const Process& b) {
        return a.id < b.id;
    });

    for (auto& p : completedProcesses) {
        // Final calculation based on completion and start times
        p.turnaroundTime = p.completionTime - p.arrivalTime;
        p.waitingTime = p.turnaroundTime - p.burstTime; 

        if (p.waitingTime < 0) p.waitingTime = 0; 

        totalWaitingTime += p.waitingTime;
        totalTurnaroundTime += p.turnaroundTime;
        if (p.responseTime != -1) {
            totalResponseTime += p.responseTime; 
            validResponseTimes++;
        }
    }

    int n = completedProcesses.size();
    result.totalProcessesCompleted = n;
    result.totalElapsedTime = currentTime;
    result.averageWaitingTime = totalWaitingTime / n;
    result.averageTurnaroundTime = totalTurnaroundTime / n;
    result.averageResponseTime = (validResponseTimes > 0) ? (totalResponseTime / validResponseTimes) : 0.0;

    int totalBusyTime = currentTime - totalIdleTime;
    result.cpuUtilization = (currentTime > 0) ? (static_cast<double>(totalBusyTime) / currentTime * 100.0) : 0.0;
    result.throughput = (currentTime > 0) ? (static_cast<double>(n) / currentTime) : 0.0;
}

/**
 * Simulates the First-Come, First-Served (FCFS) scheduling algorithm.
 * Processes are executed strictly in the order of their arrival time.
 * It is non-preemptive. Processes arriving are added to a ready queue.
 * The simulation proceeds time unit by time unit, handling arrivals, execution, completion, and idle time.
 */
SimulationResult runFCFS(std::vector<Process> processes) {
    SimulationResult result("FCFS");
    std::vector<Process> completedProcesses;
    completedProcesses.reserve(processes.size());
    std::list<Process*> readyQueue;
    int currentTime = 0;
    int totalIdleTime = 0;
    size_t processIdx = 0;
    int completedCount = 0;

    // Sort processes by arrival time initially
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime) return a.arrivalTime < b.arrivalTime;
        return a.id < b.id;
    });

    Process* runningProcess = nullptr;

    while (completedCount < processes.size()) {
        // Add newly arrived processes to the ready queue
        while (processIdx < processes.size() && processes[processIdx].arrivalTime <= currentTime) {
            readyQueue.push_back(&processes[processIdx]);
            processIdx++;
        }

        // If CPU is idle and ready queue is not empty, start the next process
        if (runningProcess == nullptr && !readyQueue.empty()) {
            runningProcess = readyQueue.front();
            readyQueue.pop_front();
            
            if (runningProcess->startTime == -1) {
                runningProcess->startTime = currentTime;
                runningProcess->responseTime = runningProcess->startTime - runningProcess->arrivalTime;
            }
        }

        // If CPU is running a process
        if (runningProcess != nullptr) {
            currentTime++;
            runningProcess->remainingBurstTime--;

            // Check for completion
            if (runningProcess->remainingBurstTime == 0) {
                runningProcess->completionTime = currentTime;
                completedProcesses.push_back(*runningProcess);
                completedCount++;
                runningProcess = nullptr; // CPU becomes free
            }
        } else { 
            // CPU is idle
            if (processIdx < processes.size() || !readyQueue.empty()) {
                 // If there are processes waiting or yet to arrive, but not ready right now, advance time to the next arrival if ready queue is empty
                if(readyQueue.empty() && processIdx < processes.size()) {
                    int idleStart = currentTime;
                    currentTime = processes[processIdx].arrivalTime;
                    totalIdleTime += (currentTime - idleStart);
                } else {
                     currentTime++;
                     totalIdleTime++;
                }
            } else {
                 break; 
            }
        }
    }

    calculateMetrics(result, completedProcesses, currentTime, totalIdleTime);
    return result;
}

/**
 * Simulates the non-preemptive Shortest Job First (SJF) scheduling algorithm.
 * When the CPU is free, the process in the ready queue with the shortest original burst time is selected to run. Once started, it runs to completion.
 * Handles arrivals, execution based on shortest burst, completion, and idle time.
 */
SimulationResult runSJF(std::vector<Process> processes) {
    SimulationResult result("SJF (Non-Preemptive)");
    std::vector<Process> completedProcesses;
    completedProcesses.reserve(processes.size());
    std::list<Process*> readyQueue; // Use pointers
    int currentTime = 0;
    int totalIdleTime = 0;
    size_t processIdx = 0;
    int completedCount = 0;

    // Sort initially by arrival time for processing arrivals correctly
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime) return a.arrivalTime < b.arrivalTime;
        return a.id < b.id;
    });

    Process* runningProcess = nullptr;

    while (completedCount < processes.size()) {
        // Add newly arrived processes to the ready queue
        while (processIdx < processes.size() && processes[processIdx].arrivalTime <= currentTime) {
            readyQueue.push_back(&processes[processIdx]);
            processIdx++;
        }

        // If CPU is idle and ready queue is not empty, select the shortest job
        if (runningProcess == nullptr && !readyQueue.empty()) {
            // Find shortest job in ready queue
            auto shortestIt = std::min_element(readyQueue.begin(), readyQueue.end(), 
                [](const Process* a, const Process* b) {
                    if(a->burstTime != b->burstTime) return a->burstTime < b->burstTime;
                    return a->arrivalTime < b->arrivalTime;
            });
            runningProcess = *shortestIt;
            readyQueue.erase(shortestIt);

            if (runningProcess->startTime == -1) {
                runningProcess->startTime = currentTime;
                runningProcess->responseTime = runningProcess->startTime - runningProcess->arrivalTime;
            }
        }

        // If CPU is running a process
        if (runningProcess != nullptr) {
            int timeToRun = runningProcess->burstTime; // Run the whole burst
            // Update current time
            currentTime += timeToRun;
            runningProcess->remainingBurstTime = 0;
            runningProcess->completionTime = currentTime;
            completedProcesses.push_back(*runningProcess);
            completedCount++;
            runningProcess = nullptr; // CPU becomes free
        } else { 
            // CPU is idle
             if (processIdx < processes.size() || !readyQueue.empty()) {
                if(readyQueue.empty() && processIdx < processes.size()) {
                    int idleStart = currentTime;
                    currentTime = processes[processIdx].arrivalTime;
                    totalIdleTime += (currentTime - idleStart);
                } else {
                     currentTime++;
                     totalIdleTime++;
                }
             } else {
                 break; // No more processes
             }
        }
    }

    calculateMetrics(result, completedProcesses, currentTime, totalIdleTime);
    return result;
}

/**
 * Simulates the preemptive Shortest Remaining Time First (SRTF) scheduling algorithm.
 * At any time unit (or upon arrival), the process with the shortest remaining burst time among the running process and all processes in the ready list is selected.
 * If a new arrival has a shorter remaining time than the currently running process, the running process is preempted.
 */
SimulationResult runSRTF(std::vector<Process> processes) {
    SimulationResult result("SRTF (Preemptive SJF)");
    std::vector<Process> completedProcesses;
    completedProcesses.reserve(processes.size());
    std::list<Process*> readyList;
    int currentTime = 0;
    int totalIdleTime = 0;
    size_t processIdx = 0;
    int completedCount = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime) return a.arrivalTime < b.arrivalTime;
        return a.id < b.id;
    });

    Process* runningProcess = nullptr;

    while (completedCount < processes.size()) {
        // Add newly arrived processes
        while (processIdx < processes.size() && processes[processIdx].arrivalTime <= currentTime) {
            readyList.push_back(&processes[processIdx]);
            processIdx++;
        }

        // Find the process with the shortest remaining time from ready list + potentially running process
        Process* shortestProcess = nullptr;
        int minRemainingTime = std::numeric_limits<int>::max();

        // Consider currently running process for preemption
        if (runningProcess) {
             shortestProcess = runningProcess;
             minRemainingTime = runningProcess->remainingBurstTime;
        }
        
        // Find shortest in ready list
        for (Process* p : readyList) {
             if (p->remainingBurstTime < minRemainingTime) {
                 minRemainingTime = p->remainingBurstTime;
                 shortestProcess = p;
             } else if (p->remainingBurstTime == minRemainingTime) {
                 // Tie-breaking: prefer the one already running, then by arrival time
                 if (shortestProcess == runningProcess || p->arrivalTime < shortestProcess->arrivalTime) {
                     shortestProcess = p;
                 } 
             }
        }

        // Preemption check or new process selection
        if (shortestProcess != nullptr && shortestProcess != runningProcess) {
            if (runningProcess != nullptr) { // Preempt
                readyList.push_back(runningProcess); // Put running process back in ready list
            }
            runningProcess = shortestProcess;
             // Remove the selected process from ready list
            readyList.remove(runningProcess);
            
            if (runningProcess->startTime == -1) { 
                runningProcess->startTime = currentTime;
                runningProcess->responseTime = runningProcess->startTime - runningProcess->arrivalTime;
            }
        }

        // Execute or handle idle time
        if (runningProcess != nullptr) {
            currentTime++;
            runningProcess->remainingBurstTime--;

            if (runningProcess->remainingBurstTime == 0) {
                runningProcess->completionTime = currentTime;
                completedProcesses.push_back(*runningProcess);
                completedCount++;
                runningProcess = nullptr; // CPU is free
            }
        } else {
            // CPU is idle
            if (processIdx < processes.size() || !readyList.empty()) {
                 if(readyList.empty() && processIdx < processes.size()) {
                    int idleStart = currentTime;
                    currentTime = processes[processIdx].arrivalTime;
                    totalIdleTime += (currentTime - idleStart);
                } else {
                     currentTime++; 
                     totalIdleTime++;
                }
            } else {
                break; // No more processes
            }
        }
    }

    calculateMetrics(result, completedProcesses, currentTime, totalIdleTime);
    return result;
}

// Helper for priority comparison (lower number means higher priority)
auto comparePriority = [](const Process* a, const Process* b) {
    if (a->priority != b->priority) return a->priority < b->priority;
    return a->arrivalTime < b->arrivalTime;
};

/**
 * Simulates the Priority scheduling algorithm (preemptive or non-preemptive).
 * Selects processes based on priority (lower number means higher priority).
 * If non-preemptive, a selected process runs to completion.
 * If preemptive, a running process can be interrupted if a higher priority process arrives.
 * Tie-breaking is done by arrival time.
 */
SimulationResult runPriority(std::vector<Process> processes, bool preemptive) {
    SimulationResult result(preemptive ? "Priority (Preemptive)" : "Priority (Non-Preemptive)");
    std::vector<Process> completedProcesses;
    completedProcesses.reserve(processes.size());
    std::list<Process*> readyList;
    int currentTime = 0;
    int totalIdleTime = 0;
    size_t processIdx = 0;
    int completedCount = 0;

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime) return a.arrivalTime < b.arrivalTime;
        return a.id < b.id;
    });

    Process* runningProcess = nullptr;

    while (completedCount < processes.size()) {
        // Add newly arrived processes
        while (processIdx < processes.size() && processes[processIdx].arrivalTime <= currentTime) {
            readyList.push_back(&processes[processIdx]);
            processIdx++;
        }

        if (preemptive) {
            // Preemptive logic: Find highest priority process
            Process* highestPriorityProcess = nullptr;
            int minPriority = std::numeric_limits<int>::max();

            if (runningProcess) {
                highestPriorityProcess = runningProcess;
                minPriority = runningProcess->priority;
            }

            for (Process* p : readyList) {
                 if (p->priority < minPriority) {
                     minPriority = p->priority;
                     highestPriorityProcess = p;
                 } else if (p->priority == minPriority) {
                     if (highestPriorityProcess == runningProcess || p->arrivalTime < highestPriorityProcess->arrivalTime) {
                         highestPriorityProcess = p;
                     }
                 }
            }

            // Preemption check
            if (highestPriorityProcess != nullptr && highestPriorityProcess != runningProcess) {
                if (runningProcess != nullptr) {
                    readyList.push_back(runningProcess);
                }
                runningProcess = highestPriorityProcess;
                readyList.remove(runningProcess);

                if (runningProcess->startTime == -1) {
                    runningProcess->startTime = currentTime;
                    runningProcess->responseTime = runningProcess->startTime - runningProcess->arrivalTime;
                }
            }
        } else {
            // Non-preemptive logic: Only select when CPU is idle
            if (runningProcess == nullptr && !readyList.empty()) {
                 auto highestPriorityIt = std::min_element(readyList.begin(), readyList.end(), comparePriority);
                 runningProcess = *highestPriorityIt;
                 readyList.erase(highestPriorityIt);

                 if (runningProcess->startTime == -1) {
                     runningProcess->startTime = currentTime;
                     runningProcess->responseTime = runningProcess->startTime - runningProcess->arrivalTime;
                 }
            }
        }

        // Execute or handle idle time
        if (runningProcess != nullptr) {
            if (preemptive) {
                currentTime++;
                runningProcess->remainingBurstTime--;

                if (runningProcess->remainingBurstTime == 0) {
                    runningProcess->completionTime = currentTime;
                    completedProcesses.push_back(*runningProcess);
                    completedCount++;
                    runningProcess = nullptr;
                }
            } else {
                // Non-preemptive runs to completion
                int timeToRun = runningProcess->burstTime;
                currentTime += timeToRun;
                runningProcess->remainingBurstTime = 0;
                runningProcess->completionTime = currentTime;
                completedProcesses.push_back(*runningProcess);
                completedCount++;
                runningProcess = nullptr;
            }
        } else {
             // CPU is idle
            if (processIdx < processes.size() || !readyList.empty()) {
                 if(readyList.empty() && processIdx < processes.size()) {
                    int idleStart = currentTime;
                    currentTime = processes[processIdx].arrivalTime;
                    totalIdleTime += (currentTime - idleStart);
                } else {
                     currentTime++; 
                     totalIdleTime++;
                }
            } else {
                break; // No more processes
            }
        }
    }

    calculateMetrics(result, completedProcesses, currentTime, totalIdleTime);
    return result;
}

/**
 * Simulates the Round Robin (RR) scheduling algorithm.
 * Processes are placed in a FIFO ready queue. The currently running process executes for a fixed time quantum. 
 * If not completed, it's preempted and placed at the end of the ready queue. 
 * Handles arrivals, execution in time slices, preemption, completion, and idle time.
 */
SimulationResult runRoundRobin(std::vector<Process> processes, int timeQuantum) {
    SimulationResult result("Round Robin (Quantum=" + std::to_string(timeQuantum) + ")");
    std::vector<Process> completedProcesses;
    completedProcesses.reserve(processes.size());
    std::queue<Process*> readyQueue;
    int currentTime = 0;
    int totalIdleTime = 0;
    size_t processIdx = 0;
    int completedCount = 0;
    int timeSliceCounter = 0; // Tracks time used in current quantum

    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime) return a.arrivalTime < b.arrivalTime;
        return a.id < b.id;
    });

    Process* runningProcess = nullptr;

    while (completedCount < processes.size()) {
        // Add newly arrived processes to the ready queue
        while (processIdx < processes.size() && processes[processIdx].arrivalTime <= currentTime) {
            readyQueue.push(&processes[processIdx]);
            processIdx++;
        }

        // If CPU is idle, get the next process from ready queue
        if (runningProcess == nullptr && !readyQueue.empty()) {
            runningProcess = readyQueue.front();
            readyQueue.pop();
            timeSliceCounter = 0; // Reset quantum timer

            if (runningProcess->startTime == -1) {
                runningProcess->startTime = currentTime;
                runningProcess->responseTime = runningProcess->startTime - runningProcess->arrivalTime;
            }
        }

        // Execute or handle idle time
        if (runningProcess != nullptr) {
            currentTime++;
            runningProcess->remainingBurstTime--;
            timeSliceCounter++;

            // Check for completion
            if (runningProcess->remainingBurstTime == 0) {
                runningProcess->completionTime = currentTime;
                completedProcesses.push_back(*runningProcess);
                completedCount++;
                runningProcess = nullptr; // CPU becomes free
                timeSliceCounter = 0;
            } 
            // Check if time quantum expired
            else if (timeSliceCounter == timeQuantum) {
                // Add newly arrived processes that arrived during this time slice before putting the current process back
                 while (processIdx < processes.size() && processes[processIdx].arrivalTime <= currentTime) {
                    readyQueue.push(&processes[processIdx]);
                    processIdx++;
                }
                // Put the current process back in the queue if it's not finished
                 readyQueue.push(runningProcess);
                 runningProcess = nullptr;
                 timeSliceCounter = 0;
            }
        } else {
            // CPU is idle
            if (processIdx < processes.size() || !readyQueue.empty()) {
                if (readyQueue.empty() && processIdx < processes.size()) {
                     int idleStart = currentTime;
                     currentTime = processes[processIdx].arrivalTime;
                     totalIdleTime += (currentTime - idleStart);
                 } else {
                    currentTime++;
                    totalIdleTime++;
                 }
            } else {
                break; // No more processes
            }
        }
    }

    calculateMetrics(result, completedProcesses, currentTime, totalIdleTime);
    return result;
}

/** Helper structure for MLFQ to store process pointer and its MLFQ state.*/
struct MlfqProcessData {
    Process* p;
    int currentQueue;
    int timeInCurrentQuantum; 
    int lastExecutionTime;

MlfqProcessData(Process* proc) : 
    p(proc), currentQueue(0), timeInCurrentQuantum(0), lastExecutionTime(proc->arrivalTime) {}
};

/**
 * Simulates the Multi-Level Feedback Queue (MLFQ) scheduling algorithm.
 * Implements a specific MLFQ configuration with 3 queues:
 * - Q0: RR with Quantum 8
 * - Q1: RR with Quantum 16
 * - Q2: FCFS
 * New processes enter Q0. Processes are demoted if they use their full quantum.
 * Higher priority queues are always processed first. 
 * Includes preemption based on arrivals in higher queues and an aging mechanism (priority boost to Q0 after 50 time units of waiting).
 */
SimulationResult runMLFQ(std::vector<Process> processes) {
    SimulationResult result("MLFQ (Q0:RR8, Q1:RR16, Q2:FCFS, Age:50)");
    const int NUM_QUEUES = 3;
    const int QUANTUM_Q0 = 8;
    const int QUANTUM_Q1 = 16;
    const int AGING_THRESHOLD = 50; 

    std::vector<Process> completedProcesses;
    completedProcesses.reserve(processes.size());
    std::vector<std::queue<MlfqProcessData*>> readyQueues(NUM_QUEUES);
    std::vector<MlfqProcessData> processDataStore;
    processDataStore.reserve(processes.size());

    int currentTime = 0;
    int totalIdleTime = 0;
    size_t processIdx = 0;
    int completedCount = 0;

    // Sort processes by arrival time initially
    std::sort(processes.begin(), processes.end(), [](const Process& a, const Process& b) {
        if (a.arrivalTime != b.arrivalTime) return a.arrivalTime < b.arrivalTime;
        return a.id < b.id;
    });

    // Create the storage for MLFQ data
    for (size_t i = 0; i < processes.size(); ++i) {
        processDataStore.emplace_back(&processes[i]);
    }

    MlfqProcessData* runningProcessData = nullptr;

    while (completedCount < processes.size()) {
        // 1. Add newly arrived processes to the highest priority queue (Q0)
        while (processIdx < processDataStore.size() && processDataStore[processIdx].p->arrivalTime <= currentTime) {
             processDataStore[processIdx].currentQueue = 0;
             processDataStore[processIdx].lastExecutionTime = currentTime; // Or arrivalTime, reset on boost
             readyQueues[0].push(&processDataStore[processIdx]);
             processIdx++;
        }

        // 2. Priority Boost (Aging)
        // Iterate through processes in lower queues (Q1, Q2) 
        for (int qLevel = 1; qLevel < NUM_QUEUES; ++qLevel) {
            int currentQueueSize = readyQueues[qLevel].size();
            for(int i = 0; i < currentQueueSize; ++i) {
                 MlfqProcessData* pData = readyQueues[qLevel].front();
                 readyQueues[qLevel].pop();
                 if (currentTime - pData->lastExecutionTime >= AGING_THRESHOLD) {
                     pData->currentQueue = 0; // Boost to highest queue
                     pData->timeInCurrentQuantum = 0;
                     pData->lastExecutionTime = currentTime; // Reset timer on boost
                     readyQueues[0].push(pData);
                 } else {
                     readyQueues[qLevel].push(pData); // Put back if not aged
                 }
            }
        }

        // 3. Select process to run
        if (runningProcessData == nullptr) {
            for (int qLevel = 0; qLevel < NUM_QUEUES; ++qLevel) {
                if (!readyQueues[qLevel].empty()) {
                    runningProcessData = readyQueues[qLevel].front();
                    readyQueues[qLevel].pop();
                    runningProcessData->timeInCurrentQuantum = 0; // Reset quantum usage

                    if (runningProcessData->p->startTime == -1) {
                        runningProcessData->p->startTime = currentTime;
                        runningProcessData->p->responseTime = runningProcessData->p->startTime - runningProcessData->p->arrivalTime;
                    }
                    break; // Found process, stop searching queues
                }
            }
        }

        // 4. Execute or handle idle time
        if (runningProcessData != nullptr) {
            currentTime++;
            runningProcessData->p->remainingBurstTime--;
            runningProcessData->timeInCurrentQuantum++;
            runningProcessData->lastExecutionTime = currentTime; // Update last run time

            // Check for completion
            if (runningProcessData->p->remainingBurstTime == 0) {
                runningProcessData->p->completionTime = currentTime;
                completedProcesses.push_back(*(runningProcessData->p));
                completedCount++;
                runningProcessData = nullptr; // CPU is free
            } else {
                 // Check for preemption due to new arrival in higher queue
                bool preempted = false;
                while (processIdx < processDataStore.size() && processDataStore[processIdx].p->arrivalTime <= currentTime) {
                    processDataStore[processIdx].currentQueue = 0;
                    processDataStore[processIdx].lastExecutionTime = currentTime;
                    readyQueues[0].push(&processDataStore[processIdx]);
                    processIdx++;
                    if (runningProcessData->currentQueue > 0) { // Only preempt if new arrival is higher priority
                        preempted = true;
                    }
                }
                
                if (preempted) {
                    // Put running process back in its queue
                    readyQueues[runningProcessData->currentQueue].push(runningProcessData);
                    runningProcessData = nullptr; 
                } else {
                     // Check for demotion (quantum expiry)
                     int currentQuantum = (runningProcessData->currentQueue == 0) ? QUANTUM_Q0 : ((runningProcessData->currentQueue == 1) ? QUANTUM_Q1 : -1); 
                     // Q2 is FCFS, no quantum

                     if (currentQuantum != -1 && runningProcessData->timeInCurrentQuantum == currentQuantum) {
                         int nextQueue = std::min(runningProcessData->currentQueue + 1, NUM_QUEUES - 1);
                         runningProcessData->currentQueue = nextQueue;
                         runningProcessData->timeInCurrentQuantum = 0;
                         readyQueues[nextQueue].push(runningProcessData);
                         runningProcessData = nullptr; // Yield CPU
                     } 
                     // If Q2 (FCFS), just continue running until completion or higher arrival
                }
            }
        } else {
            // CPU is idle
             if (processIdx < processDataStore.size() || completedCount < processes.size()) {
                 // Check if any ready queues have items. If yes, idle for 1 unit hoping something happens.
                 bool anyReady = false;
                 for(int q=0; q<NUM_QUEUES; ++q) if(!readyQueues[q].empty()) anyReady = true;

                 if (anyReady) {
                     currentTime++;
                     totalIdleTime++;
                 } else if (processIdx < processDataStore.size()) {
                    // Advance time to next arrival
                    int idleStart = currentTime;
                    currentTime = processDataStore[processIdx].p->arrivalTime;
                    totalIdleTime += (currentTime - idleStart);
                 } else {
                      // Should only happen if all processes completed? Let main loop handle termination.
                       break; 
                 }
             } else {
                 break; // No more processes to arrive or run
             }
        }
    }

    calculateMetrics(result, completedProcesses, currentTime, totalIdleTime);
    return result;
}

/**
 * Prints a formatted comparison table for multiple simulation results.
 * Takes a vector of SimulationResult objects and displays their key performance 
 * metrics side-by-side in a clearly formatted table for easy comparison.
 */
void printComparison(const std::vector<SimulationResult>& results) {
    if (results.empty()) return;

    std::cout << "\n--- Performance Comparison ---" << std::endl;
    std::cout << std::left << std::setw(45) << "Algorithm" 
              << std::setw(15) << "Avg Waiting" 
              << std::setw(15) << "Avg Turnaround" 
              << std::setw(15) << "Avg Response"
              << std::setw(15) << "CPU Util (%)"
              << std::setw(25) << "Throughput (proc/unit)"
              << std::endl;
    std::cout << std::string(130, '-') << std::endl;

    std::cout << std::fixed << std::setprecision(2);
    for (const auto& result : results) {
        std::cout << std::left << std::setw(45) << result.algorithmName
                  << std::setw(15) << result.averageWaitingTime
                  << std::setw(15) << result.averageTurnaroundTime
                  << std::setw(15) << result.averageResponseTime
                  << std::setw(15) << result.cpuUtilization
                  << std::setw(25) << result.throughput
                  << std::endl;
    }
    std::cout << "----------------------------" << std::endl;
} 