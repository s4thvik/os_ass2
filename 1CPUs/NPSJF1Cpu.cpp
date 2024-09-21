#include <bits/stdc++.h>
#include "parseLine1.h"

using namespace std;
vector<int> parseLine1(const std::string& line);


void nonpremsjf1(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));
    int time = 0;
    int cpu = 0; // To track the CPU number

    // Initialize the arrival times in details
    for (int i = 0; i < noproc; i++) {
        details[i][0] = data[i][0];
    }

    // Priority queue for ready processes (shortest job first, non-preemptive)
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>> pq;

    // Wait queue for processes undergoing I/O
    queue<pair<int, pair<int, int>>> wait;

    // Add the first process to the priority queue
    int ind = 1;
    pq.push({data[0][1], {0, 1}});  // (run time, {process number, next index})

    // Vector to keep track of the process execution details
    vector<tuple<int, int, int, int, int>> processDetails; 

    while (!pq.empty() || !wait.empty() || ind < noproc) {
        // If there are no ready processes in the pq, advance time to the next process arrival or wait completion
        if (pq.empty() && !wait.empty()) {
            time = wait.front().first; // Jump to the time when the first wait process finishes
        } else if (pq.empty() && wait.empty() && ind < noproc) {
            time = data[ind][0]; // Advance time to the next process arrival
        }

        // Process the current shortest job in the priority queue
        if (!pq.empty()) {
            pair<int, pair<int, int>> p = pq.top();
            pq.pop();

            int rt = p.first;   // remaining time (run time)
            int prNo = p.second.first;  // process number
            int prPo = p.second.second; // current position in the data array

            int startTime = time; // Start time for the current burst
            time += rt;  // Advance the time by the run time of this process
            int endTime = time; // End time for the current burst
            details[prNo][2] += rt;

            if (data[prNo][prPo + 1] == -1) {
                details[prNo][1] = time;  
            } else {
                wait.push({time + data[prNo][prPo + 1], {prNo, prPo + 1}});
            }

            // Record the process execution details
            processDetails.push_back(make_tuple(cpu, prNo, prPo, startTime, endTime));
        }

        while (!wait.empty() && wait.front().first <= time) {
            pair<int, pair<int, int>> k = wait.front();
            wait.pop();

            if (data[k.second.first][k.second.second + 1] == -1) {
                details[k.second.first][1] = time;
            } else {
                pq.push({data[k.second.first][k.second.second + 1], {k.second.first, k.second.second + 1}});
            }
        }

        while (ind < noproc && data[ind][0] <= time) {
            pq.push({data[ind][1], {ind, 1}});
            ind++;
        }

        if (pq.empty() && wait.empty() && ind < noproc) {
            time = data[ind][0];
        }
    }

    // Calculate makespan and waiting time
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }

    // Output the result
    cout << "CPU0" << endl;
    for (const auto& pd : processDetails) {
        int cpuNum, prNo, prPo, startTime, endTime;
        tie(cpuNum, prNo, prPo, startTime, endTime) = pd;
        // Adjust process burst number for display
        float procBurst = (float)(prPo + 1) / 2; 
        cout << "P" << prNo + 1 << "," << (int)procBurst << "\t" << startTime << "\t" << endTime - 1 << endl;
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

void runNPSJF1Cpu(const std::string& workloadFile) {
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

    nonpremsjf1(names, data);

}
