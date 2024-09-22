#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <algorithm>  // For sorting
#include <map>  // For maps
#include <queue>  // For queues
#include "RR2Cpu.h"
#include <iostream>
#include "parseLine2.h"
using namespace std;


// Structure to store CPU run details
struct CPURun {
    int pid;           // Process ID
    int run_number;    // Run count for the process
    int start_time;    // Start time of the run
    int end_time;      // End time of the run
    int cpu_burst;   // CPU burst for this run
};
vector<int> parseLine2(const std::string& line);


void RR2(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT, I/O
    int time = 0, quantum = 10, count1 = 0, count2 = 0;
    vector<pair<int, int>> CPU(2, {0, -1});  // (remaining time, process id)
    queue<pair<int, int>> pq;               // (remaining time, process id)
    map<int, vector<int>> wait;             // stores waiting processes with time as key
    int allarr = 0;
    vector<int> indices(noproc, 0);

    // Initialize the first process on CPU 0
    if (!data.empty()) {
        indices[0] = 1;
        CPU[0] = {data[0][1], 0};  // first process starts on CPU 0
        details[0][0] = data[0][0]; // Arrival time
        details[0][2] = data[0][1]; // Run time
    }

    // Initialize CPU run tracking
    vector<CPURun> CPU0_runs;
    vector<CPURun> CPU1_runs;
    // Track current running process and start time for each CPU
    int CPU0_current_pid = 0;
    int CPU1_current_pid = -1;
    // int CPU0_run_start_time = 0;
    // int CPU1_run_start_time = 0;
    // Map to track run numbers for each process
    map<int, int> process_run_number;
    process_run_number[0] = 1; // First run for process 0

    // Initialize the first run for CPU0
    CPU0_runs.push_back(CPURun{0, 1, 0, -1, 1});  // Add initial CPU burst to CPU0 runs 

    while (CPU[0].first > 0 || CPU[1].first > 0 || !pq.empty() || !wait.empty() || allarr == 0) {
        time++;

        // Handling waiting processes at current time
        if (wait.size() > 0) {
            if (wait.find(time) != wait.end()) {
                for (int i = 0; i < (int)wait[time].size(); i++) {  // Corrected size access
                    int pid = wait[time][i];
                    indices[pid]++;
                    if (indices[pid] < (int)data[pid].size() && data[pid][indices[pid]] != -1) {
                        pq.push({data[pid][indices[pid]], pid}); // Add next CPU burst to queue
                        details[pid][2] += data[pid][indices[pid]];  // Update Run Time
                    } else {
                        details[pid][1] = time;  // Completion time
                    }
                }
                wait.erase(time);
            }
        }

        // Check if all processes have arrived
        if (allarr == 0) {
            for (int i = 0; i < noproc; i++) {
                if (time == data[i][0] && indices[i] == 0) { 
                    pq.push({data[i][1], i});
                    indices[i] = 1;
                    details[i][0] = time;  // Arrival time
                    details[i][2] += data[i][1];  // Update Run Time
                }
            }
            bool all_arrived = true;
            for (int i = 0; i < noproc; i++) {
                if (indices[i] == 0) {
                    all_arrived = false;
                    break;
                }
            }
            if (all_arrived) allarr = 1;
        }

        // Decrease time on CPUs
        if (CPU[0].first > 0) {
            CPU[0].first--;
            count1++;
        }
        if (CPU[1].first > 0) {
            CPU[1].first--;
            count2++;
        }

        // When CPU time is 0 or quantum expired, load the next process from the queue
        if (CPU[0].first == 0 || CPU[1].first == 0 || count1 == quantum || count2 == quantum) {
            pair<int, int> p1 = CPU[0], p2 = CPU[1];
            int ind1 = p1.second, ind2 = p2.second;
            int rt1 = p1.first, rt2 = p2.first;

            // Handle CPU0
            if (rt1 == 0 && count1 <= quantum) {
                count1 = 0;
                // Record end time of the current run on CPU0
                if (CPU0_current_pid != -1) {
                    CPU0_runs.back().end_time = time;
                }
                CPU[0] = {0, -1}; // CPU0 becomes idle
                CPU0_current_pid = -1;
            }

            // Handle CPU1
            if (rt2 == 0 && count2 <= quantum) {
                count2 = 0;
                // Record end time of the current run on CPU1
                if (CPU1_current_pid != -1) {
                    CPU1_runs.back().end_time = time;
                }
                CPU[1] = {0, -1}; // CPU1 becomes idle
                CPU1_current_pid = -1;
            }

            // Handle completion or I/O for CPU0
            if (rt1 == 0 && ind1 >= 0) {
                indices[ind1]++;
                if (indices[ind1] < (int)data[ind1].size() && data[ind1][indices[ind1]] != -1) {
                    // Check if next is CPU burst or IO burst
                    if (indices[ind1] % 2 == 1) {  //  Even indices are I/O, odd are CPU 
                        wait[time + data[ind1][indices[ind1]]].push_back(ind1);  // Waiting for I/O
                        details[ind1][5] += data[ind1][indices[ind1]]; // Update I/O time
                    } else {
                        wait[time + data[ind1][indices[ind1]]].push_back(ind1);  // Waiting for CPU burst
                    }
                } else {
                    details[ind1][1] = time;  // Completion time
                }
            }

            // Handle completion or I/O for CPU1
            if (rt2 == 0 && ind2 >= 0) {
                indices[ind2]++;
                if (indices[ind2] < (int)data[ind2].size() && data[ind2][indices[ind2]] != -1) {
                    // Check if next is CPU burst or IO burst
                    if (indices[ind2] % 2 == 1) {  //  Even indices are I/O, odd are CPU 
                        wait[time + data[ind2][indices[ind2]]].push_back(ind2);  // Waiting for I/O
                        details[ind2][5] += data[ind2][indices[ind2]]; // Update I/O time
                    } else {
                        wait[time + data[ind2][indices[ind2]]].push_back(ind2);  // Waiting for CPU burst
                    }
                } else {
                    details[ind2][1] = time;  // Completion time
                }
            }

            // Assign next process to CPU0 if available
            if (CPU[0].first == 0 && !pq.empty()) {
                pair<int, int> next_p = pq.front();
                pq.pop();
                CPU[0] = next_p;
                count1 = 0;

                // Update CPU0 run statistics
                if (CPU0_current_pid != next_p.second) {
                    if (CPU0_current_pid != -1) {
                        CPU0_runs.back().end_time = time;
                    }
                    CPU0_current_pid = next_p.second;
                    process_run_number[next_p.second]++;
                    // Calculate the CPU burst number
                    int cpu_burst_number = (indices[next_p.second] + 1) / 2; 
                    CPU0_runs.push_back(CPURun{
                        next_p.second,
                        process_run_number[next_p.second],
                        time,
                        -1,
                        cpu_burst_number  // Add the CPU burst to the run
                    });
                }
            }

            // Assign next process to CPU1 if available
            if (CPU[1].first == 0 && !pq.empty()) {
                pair<int, int> next_p = pq.front();
                pq.pop();
                CPU[1] = next_p;
                count2 = 0;

                // Update CPU1 run statistics
                if (CPU1_current_pid != next_p.second) {
                    if (CPU1_current_pid != -1) {
                        CPU1_runs.back().end_time = time;
                    }
                    CPU1_current_pid = next_p.second;
                    process_run_number[next_p.second]++;
                    // Calculate the CPU burst number
                    int cpu_burst_number = (indices[next_p.second] + 1) / 2; 
                    CPU1_runs.push_back(CPURun{
                        next_p.second,
                        process_run_number[next_p.second],
                        time,
                        -1,
                        cpu_burst_number  // Add the CPU burst to the run
                    });
                }
            }

            // Handle quantum expiration for CPU0
            if (count1 == quantum && CPU[0].second != -1) {
                // Preempt current process on CPU0
                pq.push({CPU[0].first, CPU[0].second});
                // Record end time of the current run on CPU0
                if (CPU0_current_pid != -1) {
                    CPU0_runs.back().end_time = time;
                }
                // Assign next process to CPU0
                CPU[0] = pq.front();
                pq.pop();
                count1 = 0;

                // Update CPU0 run statistics
                if (CPU0_current_pid != CPU[0].second) {
                    if (CPU0_current_pid != -1) {
                        CPU0_runs.back().end_time = time;
                    }
                    CPU0_current_pid = CPU[0].second;
                    process_run_number[CPU[0].second]++;
                    // Calculate the CPU burst number
                    int cpu_burst_number = (indices[CPU[0].second] + 1) / 2; 
                    CPU0_runs.push_back(CPURun{
                        CPU[0].second,
                        process_run_number[CPU[0].second],
                        time,
                        -1,
                        cpu_burst_number  // Add the CPU burst to the run
                    });
                }
            }

            // Handle quantum expiration for CPU1
            if (count2 == quantum && CPU[1].second != -1) {
                // Preempt current process on CPU1
                pq.push({CPU[1].first, CPU[1].second});
                // Record end time of the current run on CPU1
                if (CPU1_current_pid != -1) {
                    CPU1_runs.back().end_time = time;
                }
                // Assign next process to CPU1
                CPU[1] = pq.front();
                pq.pop();
                count2 = 0;

                // Update CPU1 run statistics
                if (CPU1_current_pid != CPU[1].second) {
                    if (CPU1_current_pid != -1) {
                        CPU1_runs.back().end_time = time;
                    }
                    CPU1_current_pid = CPU[1].second;
                    process_run_number[CPU[1].second]++;
                    // Calculate the CPU burst number
                    int cpu_burst_number = (indices[CPU[1].second] + 1) / 2; 
                    CPU1_runs.push_back(CPURun{
                        CPU[1].second,
                        process_run_number[CPU[1].second],
                        time,
                        -1,
                        cpu_burst_number  // Add the CPU burst to the run
                    });
                }
            }
        }
    }

    // After the loop, finalize any ongoing CPU runs
    if (CPU0_current_pid != -1 && CPU0_runs.back().end_time == -1) {
        CPU0_runs.back().end_time = time;
    }
    if (CPU1_current_pid != -1 && CPU1_runs.back().end_time == -1) {
        CPU1_runs.back().end_time = time;
    }

    // Output process details
    // Output CPU0 statistics
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }
    cout << "CPU0" << endl;
    for (const auto& run : CPU0_runs) {
        // Assuming process IDs start from 0, adjust if needed
        cout << "P" << (run.pid + 1) << "," << run.cpu_burst << "\t" // Corrected output
             << run.start_time << "\t" << run.end_time << "\t" << endl; // Added CPU burst
    }

    // Output CPU1 statistics
    cout << "CPU1" << endl;
    for (const auto& run : CPU1_runs) {
        // Assuming process IDs start from 0, adjust if needed
        cout << "P" << (run.pid + 1) << "," << run.cpu_burst << "\t" // Corrected output
             << run.start_time << "\t" << run.end_time << "\t" << endl; // Added CPU burst
    }
    cout << "--------------------" << endl;
    cout << "AT\tCT\tRT\tMK\tWT" << endl; // Added IO
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 5; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }


}

void runRR2Cpu(const std::string& workloadFile) {
    ifstream inputFile(workloadFile);
    string line;
    vector<vector<int>> data;
    bool insidePre = false;

    while (getline(inputFile, line)) {
        if (line.find("<pre>") != string::npos) {
            insidePre = true;
            continue;
        }
        if (line.find("</pre>") != string::npos) {
            insidePre = false;
            continue;
        }
        if (insidePre) {
            vector<int> parsedLine = parseLine2(line);
            if (!parsedLine.empty()) {
                data.push_back(parsedLine);
            }
        }
    }
    inputFile.close();

    map<int, string> names;
    names.insert({0, "Arrival Time"});
    names.insert({1, "Completion Time"});
    names.insert({2, "Run Time"});
    names.insert({3, "Makespan"});
    names.insert({4, "Waiting Time"});
    names.insert({5, "I/O Time"});

    RR2(names, data);
}