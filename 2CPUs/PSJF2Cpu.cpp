#include <iostream>
#include <vector>
#include <string>
#include <algorithm>  // For sorting
#include <map>  // For maps
#include <queue>  // For queues
#include "PSJF2Cpu.h"
#include "parseLine2.h"

using namespace std;

vector<int> parseLine2(const std::string& line);

// Preemptive Shortest Job First Scheduling Function
vector<vector<int>> premsjf2(map<int, string>& names, vector<vector<int>>& data,
                            vector<CPUStat>& cpu0_stats, vector<CPUStat>& cpu1_stats) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT, I/O Time
    int time = 0;
    vector<pair<int, int>> CPU(2, {-1, -1});  // (remaining time, process id)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;  // (remaining time, process id)
    map<int, vector<int>> wait;  // stores waiting processes with time as key
    vector<int> indices(noproc, 0); // Tracks the current burst index for each process
    vector<int> burst_counters(noproc, 0); // Counts bursts for each process (accurate tracking)

    // Initialize CPU0 with the first process (if available)
    if (noproc > 0 && data[0][0] == 0) {
        CPU[0] = {data[0][1], 0};
        details[0][0] = data[0][0]; // Arrival time
        details[0][2] += data[0][1]; // Run time
        indices[0] = 1;
        burst_counters[0] = 1; // Start with burst 1
    }

    // Initialize CPU statistics
    CPUStat initial_stat = {0, 1, 0, -1};
    cpu0_stats.push_back(initial_stat);
    pair<int, int> current_cpu0 = {0, 0}; // (pid, start_time)
    pair<int, int> current_cpu1 = {-1, -1}; // (pid, start_time)

    while (true) {
        // Check if all processes are completed
        bool allCompleted = true;
        for (int i = 0; i < noproc; i++) {
            if (details[i][1] == 0) {
                allCompleted = false;
                break;
            }
        }
        if (allCompleted) {
            break; // Exit the loop if all processes are done
        }

        time++;

        // Handle waiting processes
        if (!wait.empty() && wait.find(time) != wait.end()) {
            for (std::vector<int>::size_type i = 0; i < wait[time].size(); i++) {
                int pid = wait[time][i];
                indices[pid]++; // Move to the next burst for this process
                if (indices[pid] < static_cast<int>(data[pid].size()) && data[pid][indices[pid]] != -1) {
                    pq.push({data[pid][indices[pid]], pid}); // Add the process to the ready queue
                    details[pid][2] += data[pid][indices[pid]]; // Add burst time to the runtime
                    burst_counters[pid]++; // Increment burst counter for this process
                } else {
                    details[pid][1] = time; // Process is completed
                }
            }
            wait.erase(time);
        }

        // Process arrived at current time
        for (int i = 0; i < noproc; i++) {
            if (data[i][0] == time && details[i][1] == 0) { // Process arrived and not yet completed
                pq.push({data[i][1], i}); // Add to ready queue
                details[i][0] = time; // Arrival time
                details[i][2] += data[i][1]; // Add burst time to the runtime
                indices[i] = 1; // Move to the next burst
                burst_counters[i] = 1; // Start with burst 1
            }
        }

        // Decrease time on CPUs
        for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
            if (CPU[cpuIdx].first > 0) {
                CPU[cpuIdx].first--;
            }
        }

        pair<int, int> p1 = CPU[0], p2 = CPU[1];
        int ind1 = p1.second, ind2 = p2.second;
        int rt1 = p1.first, rt2 = p2.first;

        // Check if a process on a CPU is finished
        for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
            if (CPU[cpuIdx].first == 0 && CPU[cpuIdx].second != -1) {
                int ind = CPU[cpuIdx].second;
                indices[ind]++;
                // Check if there's another burst for the process
                if (indices[ind] < static_cast<int>(data[ind].size()) && data[ind][indices[ind]] != -1) {
                    wait[data[ind][indices[ind]] + time].push_back(ind); // Add to wait queue with arrival time
                    burst_counters[ind]++; // Increment burst counter for this process
                } else {
                    details[ind][1] = time; // Completion time
                }
                CPU[cpuIdx] = {-1, -1}; // Free the CPU
            }
        }

        // Assign processes to CPUs
        if (!pq.empty()) {
            // Assign to CPU0
            pair<int, int> top1 = pq.top();
            if (rt1 > top1.first || rt1 <= 0) {
                pq.pop();
                CPU[0] = {top1.first, top1.second};
                if (rt1 > 0) {
                    pq.push({rt1, ind1});
                }
            }

            // Assign to CPU1
            if (!pq.empty()) {
                pair<int, int> top2 = pq.top();
                if (rt2 > top2.first || rt2 <= 0) {
                    pq.pop();
                    CPU[1] = {top2.first, top2.second};
                    if (rt2 > 0) {
                        pq.push({rt2, ind2});
                    }
                }
            }
        }

        // Update CPU0 statistics
        if (current_cpu0.first != CPU[0].second) {
            if (current_cpu0.first != -1) {
                cpu0_stats.back().end_time = time - 1; // End time is before the current time
            }
            if (CPU[0].second != -1) {
                CPUStat new_stat = {CPU[0].second, burst_counters[CPU[0].second] / 2 + 1, time, -1}; // Store the current burst
                cpu0_stats.push_back(new_stat);
            }
            current_cpu0.first = CPU[0].second;
            current_cpu0.second = time;
        }

        // Update CPU1 statistics
        if (current_cpu1.first != CPU[1].second) {
            if (current_cpu1.first != -1) {
                cpu1_stats.back().end_time = time - 1; // End time is before the current time
            }
            if (CPU[1].second != -1) {
                CPUStat new_stat = {CPU[1].second, burst_counters[CPU[1].second] / 2 + 1, time, -1}; // Store the current burst
                cpu1_stats.push_back(new_stat);
            }
            current_cpu1.first = CPU[1].second;
            current_cpu1.second = time;
        }
    }

    // Close any ongoing CPU0 execution
    if (current_cpu0.first != -1) {
        cpu0_stats.back().end_time = time;
    }
    // Close any ongoing CPU1 execution
    if (current_cpu1.first != -1) {
        cpu1_stats.back().end_time = time;
    }

    return details;
}

void runPSJF2Cpu(const std::string& workloadFile) {
    ifstream inputFile(workloadFile);
    string line;
    vector<vector<int>> data;
    bool insidePre = false;

    // Read and parse the input file
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

    // Define process detail names
    map<int, string> names;
    names.insert({0, "Arrival Time"});
    names.insert({1, "Completion Time"});
    names.insert({2, "Run Time"});
    names.insert({3, "Makespan"});
    names.insert({4, "Waiting Time"});
    names.insert({5, "I/O Time"});

    // Initialize CPU statistics containers
    vector<CPUStat> cpu0_stats;
    vector<CPUStat> cpu1_stats;

    // Perform scheduling
    vector<vector<int>> details = premsjf2(names, data, cpu0_stats, cpu1_stats);

    // Output process details
    int noproc = data.size();

    // Calculate Makespan and Waiting Time
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }

    // Output CPU0 statistics
    cout << "CPU0" << endl;
    for (const auto& stat : cpu0_stats) {
        cout << "P" << stat.pid + 1 << "," << stat.burst << "\t"
             << stat.start_time << "\t" << stat.end_time << endl;
    }

    // Output CPU1 statistics
    cout << "CPU1" << endl;
    for (const auto& stat : cpu1_stats) {
        cout << "P" << stat.pid + 1 << "," << stat.burst << "\t"
             << stat.start_time << "\t" << stat.end_time << endl;
    }
    cout << "--------------------" << endl;
    cout << "AT\tCT\tRT\tMK\tWT" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 5; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }
}
