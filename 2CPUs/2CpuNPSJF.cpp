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

void nonpremsjf(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // AT, CT, RT, MK, WT

    int time = 0;
    
    // Initialize the arrival times in details
    for (int i = 0; i < noproc; i++) {
        details[i][0] = data[i][0]; // Arrival time
    }

    // Priority queue for ready processes (shortest job first, non-preemptive)
    priority_queue<pair<int, pair<int, int>>, vector<pair<int, pair<int, int>>>, greater<pair<int, pair<int, int>>>> pq;

    // Wait queue for processes undergoing I/O
    queue<pair<int, pair<int, int>>> wait;

    // Booleans to track CPU availability
    bool cpuFree[2] = {true, true}; // Both CPUs start free
    pair<int, pair<int, int>> cpuStatus[2] = {{-1, {-1, -1}}, {-1, {-1, -1}}}; // Tracking current CPU usage

    // Vector to keep track of the process execution details for each CPU
    vector<tuple<int, int, int, int, int>> processDetails[2]; // CPU-wise process details

    // Add the first process to the priority queue if exists
    int ind = 0;  // Index of the next process to be added to the ready queue
    while (ind < noproc && data[ind][0] == 0) {
        pq.push({data[ind][1], {ind, 1}});
        ind++;
    }

    while (!pq.empty() || !wait.empty() || ind < noproc || !cpuFree[0] || !cpuFree[1]) {
        // Check for process arrival
        while (ind < noproc && data[ind][0] <= time) {
            pq.push({data[ind][1], {ind, 1}});
            ind++;
        }

        // Check for I/O completion
        while (!wait.empty() && wait.front().first <= time) {
            pair<int, pair<int, int>> k = wait.front();
            wait.pop();

            // Add to ready queue after I/O completion
            if (data[k.second.first][k.second.second + 1] == -1) {
                details[k.second.first][1] = time;  // Completion time if no more phases
            } else {
                pq.push({data[k.second.first][k.second.second + 1], {k.second.first, k.second.second + 1}});
            }
        }

        // Check for each CPU if it's free, assign a process
        for (int cpu = 0; cpu < 2; cpu++) {
            if (cpuFree[cpu] && !pq.empty()) {
                // CPU is free and there are processes in the queue
                pair<int, pair<int, int>> p = pq.top();
                pq.pop();

                // Assign the process to CPU
                cpuFree[cpu] = false;
                cpuStatus[cpu] = p;
                details[p.second.first][2] += p.first;  // Add to run time

                // Record the process execution details
                processDetails[cpu].push_back(make_tuple(cpu, p.second.first, p.second.second, time, time + p.first));
            }
        }

        // Execute processes on each CPU
        for (int cpu = 0; cpu < 2; cpu++) {
            if (!cpuFree[cpu]) {
                pair<int, pair<int, int>>& currentProcess = cpuStatus[cpu];
                int& remainingTime = currentProcess.first;
                int processId = currentProcess.second.first;
                int currentPhase = currentProcess.second.second;

                remainingTime--;  // Decrement the remaining time for the process
                if (remainingTime == 0) {  // Process finished execution on this CPU
                    if (data[processId][currentPhase + 1] == -1) {
                        // No more phases, process is complete
                        details[processId][1] = time + 1;  // Set completion time
                    } else {
                        // Move to I/O phase
                        wait.push({time + data[processId][currentPhase + 1], {processId, currentPhase + 1}});
                    }
                    cpuFree[cpu] = true;  // Free the CPU after execution
                }
            }
        }

        time++;  // Increment the global time
    }

    // Calculate makespan and waiting time
    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }

    // Output CPU-wise execution details
    cout << "CPU0" << endl;
    for (const auto& pd : processDetails[0]) {
        int cpuNum, prNo, prPo, startTime, endTime;
        tie(cpuNum, prNo, prPo, startTime, endTime) = pd;
        // Adjust process burst number for display
        float procBurst = (float)(prPo + 1) / 2; 
        cout << "P" << prNo + 1 << "," << (int)procBurst << "\t" << startTime << "\t" << endTime - 1 << endl;
    }
    cout << "--------------------" << endl;
    cout << "CPU1" << endl;
    for (const auto& pd : processDetails[1]) {
        int cpuNum, prNo, prPo, startTime, endTime;
        tie(cpuNum, prNo, prPo, startTime, endTime) = pd;
        // Adjust process burst number for display
        float procBurst = (float)(prPo + 1) / 2; 
        cout << "P" << prNo + 1 << "," << (int)procBurst << "\t" << startTime << "\t" << endTime - 1 << endl;
    }
    cout << "--------------------" << endl;

    // Output final statistics
    cout << "AT\tCT\tRT\tMK\tWT" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 5; j++) {
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

    nonpremsjf(names, data);
    
    return 0;
}
