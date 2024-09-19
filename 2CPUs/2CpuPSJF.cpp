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

vector<vector<int>> premsjf(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT, IO
    int time = 0;
    vector<pair<int, int>> CPU(2, {-1, -1});  // (remaining time, process id)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;  // (remaining time, process id)
    map<int, vector<int>> wait;  // stores waiting processes with time as key
    vector<int> indices(noproc, 0);
    vector<tuple<int, int, int, int, int>> processDetails[2];  // CPU-wise process details 

    // Initialize first process on CPU 0
    if (!data.empty() && data[0][0] == 0) {
        CPU[0] = {data[0][1], 0};  // first process starts on CPU 0
        details[0][0] = data[0][0]; // Arrival time
        details[0][2] = data[0][1]; // Run time
    }

    bool allArrived = false;
    bool allProcessesCompleted = false;

    while (!allProcessesCompleted) {
        time++;

        // Handling waiting processes at current time
        if (!wait.empty() && wait.find(time) != wait.end()) {
            for (int pid : wait[time]) {
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

        // Check if all processes have arrived
        if (!allArrived) {
            bool allProcessesArrived = true;
            for (int i = 0; i < noproc; i++) {
                if (time == data[i][0]) {
                    pq.push({data[i][1], i});
                    indices[i] = 1;
                    details[i][0] = time;  // Arrival time
                }
                if (indices[i] < data[i].size()) {
                    allProcessesArrived = false;
                }
            }
            allArrived = allProcessesArrived;
        }

        // Decrease time on CPUs
        for (int cpu = 0; cpu < 2; cpu++) {
            if (CPU[cpu].first > 0) CPU[cpu].first--;
        }

        // Check if a CPU has finished its current task
        for (int cpu = 0; cpu < 2; cpu++) {
            if (CPU[cpu].first == 0 && CPU[cpu].second != -1) {
                int pid = CPU[cpu].second;
                indices[pid]++;
                if (indices[pid] < data[pid].size() && data[pid][indices[pid]] != -1) {
                    wait[data[pid][indices[pid]] + time].push_back(pid);
                } else {
                    details[pid][1] = time;  // Set completion time
                }
                CPU[cpu] = {-1, -1};  // Free the CPU
            }
        }

        // Assign new processes to CPUs if available
        for (int cpu = 0; cpu < 2; cpu++) {
            if (CPU[cpu].first == -1 && !pq.empty()) {
                pair<int, int> top = pq.top();
                pq.pop();
                CPU[cpu] = {top.first, top.second};
                processDetails[cpu].push_back({cpu, top.second, indices[top.second] - 1, time, time + top.first});
            }
        }

        // Check if all processes are completed and all CPUs are idle
        allProcessesCompleted = true;
        for (int i = 0; i < noproc; i++) {
            if (indices[i] < data[i].size()) {
                allProcessesCompleted = false;
                break;
            }
        }
        for (int cpu = 0; cpu < 2; cpu++) {
            if (CPU[cpu].first != -1) {
                allProcessesCompleted = false;
                break;
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
            cout << "P" << prNo + 1 << "," << prPo << "\t" << startTime << "\t" << endTime - 1 << endl;
        }
        cout << "--------------------" << endl;
    }

    cout << "AT\tCT\tRT\tMK\tWT\tIO" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 6; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }
    return details;
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

    vector<vector<int>> details = premsjf(names, data);
    // Output results
    int noproc = data.size();
    cout << "AT\tCT\tRT\tMK\tWT" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 5; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }
    return 0;
}