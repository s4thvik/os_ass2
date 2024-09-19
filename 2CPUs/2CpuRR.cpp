#include <bits/stdc++.h>

using namespace std;

vector<int> parseLine(const string& line) {
    vector<int> result;
    stringstream ss(line);
    int number;
    while (ss >> number) {
        result.push_back(number);
    }
    return result;
}

void roundRobin(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT, IO
    int time = 0, quantum = 2;
    vector<pair<int, int>> CPU(2, {0, -1});  // (remaining time, process id)
    queue<pair<int, int>> pq;  // (remaining time, process id)
    map<int, vector<int>> wait;  // stores waiting processes with time as key
    int allarr = 0;
    vector<int> indices(noproc, 0);
    vector<tuple<int, int, int, int, int>> processDetails[2];  // CPU-wise process details 
    vector<int> burstCounter(noproc, 0);  // Track burst number for each process

    // Initialize first process on CPU 0
    if (!data.empty()) {
        indices[0] = 1;
        CPU[0] = {data[0][1], 0};  // first process starts on CPU 0
        details[0][0] = data[0][0]; // Arrival time
        details[0][2] += data[0][1]; // Run time
        processDetails[0].push_back({0, 0, burstCounter[0]++, time, time + data[0][1]});  // Log first CPU assignment
    }

    while (CPU[0].first > 0 || CPU[1].first > 0 || !pq.empty() || !wait.empty() || allarr == 0) {
        time++;

        // Handling waiting processes at current time
        if (wait.size() > 0) {
            if (wait.find(time) != wait.end()) {
                for (int i = 0; i < wait[time].size(); i++) {
                    int pid = wait[time][i];
                    indices[pid]++;
                    if (indices[pid] < data[pid].size() && data[pid][indices[pid]] != -1) {
                        pq.push({data[pid][indices[pid]], pid});
                        details[pid][2] += data[pid][indices[pid]];  // Add to run time
                    } else {
                        details[pid][1] = time;  // Completion time
                    }
                }
                wait.erase(time);
            }
        }

        // Check if all processes have arrived
        if (allarr == 0) {
            int t = 1;
            for (int i = 0; i < noproc; i++) {
                if (time == data[i][0]) {
                    pq.push({data[i][1], i});
                    indices[i] = 1;
                    details[i][0] = time;  // Arrival time
                    details[i][2] += data[i][1];  // Add to run time
                }
                t = min(t, indices[i]);
            }
            if (t > 0) allarr = 1;
        }

        // Decrease time on CPUs
        if (CPU[0].first > 0) CPU[0].first--;
        if (CPU[1].first > 0) CPU[1].first--;

        // Process switching logic
        for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
            if (CPU[cpuIdx].first == 0 && CPU[cpuIdx].second != -1) { // Process on CPU finished
                int pid = CPU[cpuIdx].second;
                indices[pid]++;
                if (indices[pid] < data[pid].size() && data[pid][indices[pid]] != -1) {
                    wait[time + data[pid][indices[pid]]].push_back(pid); // Add to waiting queue
                    details[pid][5] = data[pid][indices[pid]]; // I/O Time
                } else {
                    details[pid][1] = time;  // Completion time
                }
                CPU[cpuIdx] = {-1, -1}; // Reset CPU
            }

            // Assign new process to the CPU
            if (CPU[cpuIdx].first == 0 && !pq.empty()) {
                auto nextProcess = pq.front();
                pq.pop();
                CPU[cpuIdx] = nextProcess;
                processDetails[cpuIdx].push_back({cpuIdx, nextProcess.second, burstCounter[nextProcess.second]++, time, time + nextProcess.first});
            }
        }

        // Check if quantum has elapsed
        for (int cpuIdx = 0; cpuIdx < 2; cpuIdx++) {
            if (CPU[cpuIdx].first > 0 && (time - std::get<3>(processDetails[cpuIdx].back()) >= quantum)) {
                // Quantum elapsed, move the process back to the ready queue
                int pid = CPU[cpuIdx].second;
                int rt = CPU[cpuIdx].first;
                pq.push({rt, pid});
                CPU[cpuIdx] = {-1, -1}; // Reset CPU
            }
        }

    }

    // Calculate makespan and waiting time
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }

    // Output CPU statistics
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

    // Output process details table
    cout << "AT\tCT\tRT\tMK\tWT\tIO" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 6; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }
}

int main() {
    ifstream inputFile("process1.dat");
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
            vector<int> parsedLine = parseLine(line);
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

    roundRobin(names, data);

    return 0;
}
