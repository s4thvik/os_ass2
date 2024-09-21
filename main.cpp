#include <iostream>
#include <string>
#include <cstdlib>
using namespace std;

// Include the header files for each scheduling algorithm
#include "1Cpus/FIFO1Cpu.h"
#include "2Cpus/FIFO2Cpu.h"

#include "1Cpus/NPSJF1Cpu.h"
#include "2Cpus/NPSJF2Cpu.h"

#include "1Cpus/PSJF1Cpu.h"
#include "2Cpus/PSJF2Cpu.h"

#include "1Cpus/RR1Cpu.h"
#include "2Cpus/RR2Cpu.h"

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <scheduling-algorithm> <path-to-workload-description-file>\n";
        return 1;
    }

    int algorithmChoice = std::stoi(argv[1]);
    std::string workloadFile = argv[2];

    cout << "Scheduling Algorithms:\n";
    cout << "1: First In First Out (FIFO)\n";
    cout << "2: Non-Preemptive Shortest Job First (NPSJF)\n";
    cout << "3: Preemptive Shortest Job First (PSJF)\n";
    cout << "4: Round Robin (RR)\n\n";
    map <int, string> algoNames;
    algoNames[1] = "First In First Out (FIFO)";
    algoNames[2] = "Non-Preemptive Shortest Job First (NPSJF)";
    algoNames[3] = "Preemptive Shortest Job First (PSJF)";
    algoNames[4] = "Round Robin (RR)";
    std::cout << "You selected algorithm " << algoNames[algorithmChoice] << " with workload file: " << workloadFile << "\n";

    switch (algorithmChoice) {
        case 1:
            runFIFO1Cpu(workloadFile);
            runFIFO2Cpu(workloadFile);
            break;
        case 2:
            runNPSJF1Cpu(workloadFile);
            runNPSJF2Cpu(workloadFile);
            break;
        case 3:
            runPSJF1Cpu(workloadFile);
            runPSJF2Cpu(workloadFile);
            break;
        case 4:
            runRR1Cpu(workloadFile);
            runRR2Cpu(workloadFile);
            break;
        default:
            std::cerr << "Invalid algorithm choice. Please select 1, 2, 3, or 4.\n";
            return 1;
    }

    return 0;
}