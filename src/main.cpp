#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>
#include "Process.h"
#include "Scheduler.h"

/**
 * Parses a single line from the process input file.
 * Expects a comma-separated string with format: ID,ArrivalTime,BurstTime,Priority.
 * Converts these values to integers and constructs a Process object.
 * Throws runtime error if parsing fails or the format is incorrect.
 */
Process parseProcessLine(const std::string& line) {
    std::stringstream ss(line);
    std::string segment;
    std::vector<int> values;

    while (std::getline(ss, segment, ',')) {
        try {
            values.push_back(std::stoi(segment));
        }
        catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid integer format in line: " + line);
        }
        catch (const std::out_of_range& e) {
            throw std::runtime_error("Integer out of range in line: " + line);
        }
    }

    if (values.size() != 4) {
        throw std::runtime_error("Incorrect number of values in line: " + line + ". Expected format: ID,ArrivalTime,BurstTime,Priority");
    }

    return Process(values[0], values[1], values[2], values[3]);
}

/**
 * Loads process definitions from a specified file.
 * Reads the file line by line, skipping empty lines and lines starting with '#' (comments in test file).
 * Uses parseProcessLine to convert each valid line into a Process object.
 * Throws runtime error if the file cannot be opened or if parsing fails.
 */
std::vector<Process> loadProcessesFromFile(const std::string& filename) {
    std::ifstream infile(filename);
    if (!infile) {
        throw std::runtime_error("Error opening file: " + filename);
    }

    std::vector<Process> processes;
    std::string line;
    int lineNumber = 0;
    while (std::getline(infile, line)) {
        lineNumber++;
        line.erase(0, line.find_first_not_of(" \t\n\r"));
        line.erase(line.find_last_not_of(" \t\n\r") + 1);

        if (line.empty() || line[0] == '#') { 
            continue;
        }

        try {
            processes.push_back(parseProcessLine(line));
        } catch (const std::runtime_error& e) {
            std::cerr << "Error parsing line " << lineNumber << ": " << e.what() << std::endl;
            throw; 
        }
    }

    infile.close();
    return processes;
}

/**
 * Main entry point for the CPU scheduling simulator.
 * Handles command-line arguments to get the input process file path.
 * Loads the process definitions using loadProcessesFromFile.
 * Runs simulations for various scheduling algorithms (FCFS, SJF, SRTF, Priority, RR, MLFQ).
 * Collects the results from each simulation.
 * Prints a comparative table of the performance metrics for all algorithms.
 */
int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <process_file.txt>" << std::endl;
        return 1;
    }

    std::string filename = argv[1];
    std::vector<Process> processes;

    try {
        processes = loadProcessesFromFile(filename);
        std::cout << "Successfully loaded " << processes.size() << " processes from " << filename << std::endl;
    } catch (const std::runtime_error& e) {
        std::cerr << "Failed to load processes: " << e.what() << std::endl;
        return 1;
    }

    if (processes.empty()) {
        std::cerr << "No valid processes found in the file." << std::endl;
        return 1;
    }

    
    std::vector<SimulationResult> allResults;

    
    std::vector<Process> fcfsProcesses = processes;
    allResults.push_back(runFCFS(fcfsProcesses));

    
    std::vector<Process> sjfProcesses = processes;
    allResults.push_back(runSJF(sjfProcesses));

    
    std::vector<Process> srtfProcesses = processes;
    allResults.push_back(runSRTF(srtfProcesses));

    
    std::vector<Process> priorityNPProcesses = processes;
    allResults.push_back(runPriority(priorityNPProcesses, false));

    
    std::vector<Process> priorityPProcesses = processes;
    allResults.push_back(runPriority(priorityPProcesses, true));

    
    std::vector<Process> rrProcesses = processes;
    int timeQuantum = 4; 
    allResults.push_back(runRoundRobin(rrProcesses, timeQuantum));

    
    std::vector<Process> mlfqProcesses = processes;
    allResults.push_back(runMLFQ(mlfqProcesses));

    
    printComparison(allResults);

    return 0;
} 