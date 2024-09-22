#include <iostream>
#include <bits/stdc++.h>
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

#include "2CPUs/parseLine2.h"
#include "1CPUs/parseLine1.h"

void DetailsOne()
{
    cout << "--------------------" << endl;
    cout << "Running on 1 CPU" << endl;
    cout << "--------------------" << endl;
}

void DetailsTwo()
{
    cout << "-------------------------------------" << endl;
    cout << "Running on 2 CPUs" << endl;
    cout << "-------------------------------------" << endl;
}

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: " << argv[0] << " <scheduling-algorithm> <path-to-workload-description-file>\n";
        return 1;
    }

    int algorithmChoice = std::stoi(argv[1]);
    std::string workloadFile = argv[2];

    map <int, string> algoNames;
    algoNames[1] = "First In First Out (FIFO)";
    algoNames[2] = "Non-Preemptive Shortest Job First (NPSJF)";
    algoNames[3] = "Preemptive Shortest Job First (PSJF)";
    algoNames[4] = "Round Robin (RR)";
    std::cout << "You selected algorithm " << algoNames[algorithmChoice] << " with workload file: " << workloadFile << "\n";

    switch (algorithmChoice) {
        case 1:
            DetailsOne();
            runFIFO1Cpu(workloadFile);
            DetailsTwo();
            runFIFO2Cpu(workloadFile);
            break;
        case 2:
            DetailsOne();
            runNPSJF1Cpu(workloadFile);
            DetailsTwo();
            runNPSJF2Cpu(workloadFile);
            break;
        case 3:
            DetailsOne();
            runPSJF1Cpu(workloadFile);
            DetailsTwo();
            runPSJF2Cpu(workloadFile);
            break;
        case 4:
            DetailsOne();
            runRR1Cpu(workloadFile);
            DetailsTwo();
            runRR2Cpu(workloadFile);
            break;
        default:
            std::cerr << "Invalid algorithm choice. Please select 1, 2, 3, or 4.\n";
            return 1;
    }

    return 0;
}