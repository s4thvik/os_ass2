#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <algorithm>  // For sorting
#include <map>  // For maps
#include <queue>  // For queues
#include <iostream>
#include "parseLine1.h"

using namespace std;
vector<int> parseLine1(const std::string& line);


// Function to handle round-robin scheduling
void RR1(map<int, string> &names, vector<vector<int>> &data)
{
    int time = 0, quantum = 2;  // Set quantum time
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // AT, CT, RT, MK, WT, IO
    vector<tuple<int, int, int, int>> burstDetails;  // For storing burst details

    for (int i = 0; i < noproc; i++) {
        details[i][0] = data[i][0];  // Set arrival time
    }

    queue<pair<int, int>> pq;  // Ready queue storing {remaining time, process index}
    deque<pair<int, int>> wait;  // Wait queue for processes doing IO, storing {IO completion time, process index}
    vector<int> indices(noproc, 0);  // Track current position in the process's burst list
    indices[0] = 1;  // Process 0 starts with its first CPU burst
    pq.push({data[0][1], 0});  // Push first process into the ready queue

    bool allarr = false;  // To check if all processes have arrived

    while (!pq.empty() || !wait.empty() || !allarr) {  // Loop till all processes complete
        // Handle waiting queue for IO processes
        if (!wait.empty()) {
            auto ioProcess = wait.front();
            int io = ioProcess.first;
            int indi = ioProcess.second;
            while (time >= io && !wait.empty()) {  // Ensure the process has finished IO
                wait.pop_front();  // Remove from waiting queue

                indices[indi] += 1;
                if (data[indi][indices[indi]] == -1) {
                    details[indi][1] = time;  // Process completion time
                } else {
                    pq.push({data[indi][indices[indi]], indi});  // Add to ready queue after IO
                    details[indi][2] += data[indi][indices[indi]];  // Update run time
                }

                if (!wait.empty()) {  // Update for next waiting process
                    ioProcess = wait.front();
                    io = ioProcess.first;
                    indi = ioProcess.second;
                }
            }
        }

        // Handle ready queue
        if (!pq.empty()) {
            auto process = pq.front();
            int rt = process.first;
            int ind = process.second;
            pq.pop();

            int startTime = time;  // Record start time of the current burst
            int endTime;

            if (rt > quantum) {  // Process has more burst left after quantum
                pq.push({rt - quantum, ind});
                time += quantum;
                endTime = time;
            } else {  // Process finishes its CPU burst within the quantum
                time += rt;
                endTime = time;
                indices[ind] += 1;  // Move to next burst

                if (data[ind][indices[ind]] != -1) {  // If there's an IO burst
                    wait.push_back({time + data[ind][indices[ind]], ind});  // Add to wait queue for IO
                    details[ind][5] += data[ind][indices[ind]];  // Update IO time
                } else {
                    details[ind][1] = time;  // Completion time
                }
            }

            // Record burst details
            burstDetails.push_back(make_tuple(ind + 1, indices[ind], startTime, endTime - 1));
        }

        // Handle process arrivals
        if (!allarr) {
            allarr = true;
            for (int i = 1; i < noproc; i++) {
                if (time >= data[i][0] && indices[i] == 0) {  // If process has arrived
                    pq.push({data[i][1], i});
                    indices[i] = 1;  // Set its CPU burst index
                    details[i][0] = time;  // Record the time it starts
                    details[i][2] += data[i][1];  // Update run time
                }
                if (indices[i] == 0) {
                    allarr = false;  // Not all processes have arrived yet
                }
            }
        }

        // Idle CPU case: if no processes are ready and waiting queue is empty
        if (pq.empty() && !wait.empty()) {
            time = wait.front().first;  // Jump to the time when the next IO completes
        } else if (pq.empty() && allarr) {
            break;  // Exit when all processes are completed
        }
    }

    // Calculate completion times, makespan, and waiting times
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan
        details[i][4] = details[i][3] - details[i][2];  // Waiting time
    }

    // Output the result
    cout << "CPU0" << endl;
    for (const auto& burstDetail : burstDetails) {
        int processNo = get<0>(burstDetail);
        int burstNo = get<1>(burstDetail);
        int startTime = get<2>(burstDetail);
        int endTime = get<3>(burstDetail);
        cout << "P" << processNo << "," << burstNo << "\t" << startTime << "\t" << endTime << endl;
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

void runRR1Cpu(const std::string& workloadFile) {
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
            vector<int> parsedLine = parseLine1(line);
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

    RR1(names, data);

}
