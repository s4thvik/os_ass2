#include <iostream>
#include <bits/stdc++.h>  // This is not a good practice, consider including specific headers
#include <vector>
#include <string>
#include <algorithm>  // For sorting
#include <map>  // For maps
#include <queue>  // For queues
#include <deque> // For deque
#include <iostream>
#include "FIFO2Cpu.h"
#include "parseLine2.h"


using namespace std;
vector<int> parseLine2(const std::string& line);


vector<vector<int>> fifo2(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT, IO
    int time = 0;
    vector<pair<int, int>> CPU(2, {-1, -1});  // (remaining time, process id)
    queue<pair<int, int>, deque<pair<int, int>>> pq;  // (remaining time, process id) with deque
    map<int, vector<int>> wait;  // stores waiting processes with time as key
    vector<bool> cpuFree(2, true); // Track if CPUs are free
    vector<int> indices(noproc, 0);
    vector<tuple<int, int, int, int, int>> processDetails[2];  // CPU-wise process details 
    
    // Initialize first process on CPU 0
    if (!data.empty()) {
        indices[0] = 1;
        CPU[0] = {data[0][1], 0};  // first process starts on CPU 0
        details[0][0] = data[0][0]; // Arrival time
        details[0][2] = data[0][1]; // Run time
        cpuFree[0] = false;
        // Add the initial CPU burst for the first process
        processDetails[0].push_back({0, 0, 1, 0, data[0][1]}); 
    }

    while (any_of(cpuFree.begin(), cpuFree.end(), [](bool free) { return !free; }) || !pq.empty() || !wait.empty()) {
        time++;

        // Handle waiting processes that complete their I/O at this time
        // Handle waiting processes that complete their I/O at this time
        if (wait.find(time) != wait.end()) {
            for (std::vector<int>::size_type i = 0; i < wait[time].size(); i++) {
                int pid = wait[time][i];
                indices[pid]++;
                if (indices[pid] < static_cast<int>(data[pid].size()) && data[pid][indices[pid]] != -1) {
                    pq.push({data[pid][indices[pid]], pid});
                    details[pid][2] += data[pid][indices[pid]];  // Add to run time
                } else {
                    details[pid][1] = time;  // Completion time
                }
            }
            wait.erase(time);
        }

        // Check if any process arrives at this time
        for (int i = 0; i < noproc; i++) {
            if (time == data[i][0] && indices[i] == 0) {
                pq.push({data[i][1], i});
                indices[i] = 1;
                details[i][0] = time;  // Arrival time
                details[i][2] += data[i][1];  // Add to run time
            }
        }

        // Decrement CPU times
        for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
            if (CPU[cpuIdx].first > 0) {
                CPU[cpuIdx].first--;
                if (CPU[cpuIdx].first == 0) {
                    int ind = CPU[cpuIdx].second;
                    indices[ind]++;
                    if (indices[ind] < static_cast<int>(data[ind].size()) && data[ind][indices[ind]] != -1) {
                        wait[time + data[ind][indices[ind]]].push_back(ind);
                        details[ind][5] = data[ind][indices[ind]];  // I/O Time
                    } else {
                        details[ind][1] = time;  // Completion time
                    }
                    cpuFree[cpuIdx] = true; // CPU is free
                    CPU[cpuIdx] = {-1, -1}; // Reset CPU
                }
            }
        }


        // Assign processes to available CPUs
        for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
            if (cpuFree[cpuIdx] && !pq.empty()) {
                pair<int, int> top = pq.front();
                pq.pop();
                CPU[cpuIdx] = top;
                processDetails[cpuIdx].push_back({cpuIdx, top.second, indices[top.second], time, time + top.first});
                cpuFree[cpuIdx] = false; // CPU is busy
            }
        }
    }

    // Calculate makespan and waiting time
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }

    // Output results
    for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
        cout << "CPU" << cpuIdx << endl;
        for (const auto& pd : processDetails[cpuIdx]) {
            int cpuNum, prNo, prPo, startTime, endTime;
            tie(cpuNum, prNo, prPo, startTime, endTime) = pd;
            // Adjust process burst number for display
            float procBurst = (float)(prPo + 1) / 2;
            cout << "P" << prNo + 1 << "," << (int)procBurst << "\t" << startTime << "\t" << endTime - 1 << endl;
        }
        cout << "--------------------" << endl;
    }
    cout << "AT\tCT\tRT\tMK\tWT" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 5; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }
    return details;
}

void runFIFO2Cpu(const std::string& workloadFile) {
    ifstream inputFile(workloadFile); // Replace hardcoded file with passed file name
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

    vector<vector<int>> details = fifo2(names, data);
}