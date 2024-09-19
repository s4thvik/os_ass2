#include <bits/stdc++.h>

using namespace std;

// Define a structure to hold CPU statistics
struct CPUStat {
    int pid;        // Process ID
    int burst;      // Burst number for the process
    int start_time; // Start time of the burst
    int end_time;   // End time of the burst
};

// Function to parse a line of integers
vector<int> parseLine(const string& line) {
    vector<int> result;
    stringstream ss(line);
    int number;
    while (ss >> number) {
        result.push_back(number);
    }
    return result;
}

// Preemptive Shortest Job First Scheduling Function
vector<vector<int>> premsjf(map<int, string>& names, vector<vector<int>>& data, 
                            vector<CPUStat>& cpu0_stats, vector<CPUStat>& cpu1_stats) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT, I/O Time
    int time = 0;
    vector<pair<int, int>> CPU(2, {-1, -1});  // (remaining time, process id)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;  // (remaining time, process id)
    map<int, vector<int>> wait;  // stores waiting processes with time as key
    int allarr = 0;
    vector<int> indices(noproc, 0);
    
    // Initialize CPU0 with the first process
    CPU[0] = {data[0][1], 0};  // first process starts on CPU
    details[0][0] = data[0][0]; // Arrival time
    details[0][2] += data[0][1]; // Run time
    indices[0] = 1;
    // Initialize CPU statistics
    vector<int> process_bursts(noproc, 0); // Tracks number of bursts per process
    CPUStat initial_stat = {0, 1, 0, -1};
    cpu0_stats.push_back(initial_stat);
    pair<int, int> current_cpu0 = {0, 0}; // (pid, start_time)
    pair<int, int> current_cpu1 = {-1, -1}; // (pid, start_time)

    while (CPU[0].first > 0 || CPU[1].first > 0 || !pq.empty() || !wait.empty() || allarr == 0) {
        time++;
        // Handling waiting processes at current time
        if (wait.size() > 0) {
            if (wait.find(time) != wait.end()) {
                for (int i = 0; i < wait[time].size(); i++) {
                    int pid = wait[time][i];
                    indices[pid]++;
                    if (data[pid][indices[pid]] != -1) {
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

        pair<int, int> p1 = CPU[0], p2 = CPU[1];
        int ind1 = p1.second, ind2 = p2.second;
        int rt1 = p1.first, rt2 = p2.first;

        if(rt1 == 0 && p1.second != -1)
        {
            indices[ind1]++;
            if(data[ind1][indices[ind1]] != -1)
                wait[data[ind1][indices[ind1]] + time].push_back(ind1);
            else
                details[ind1][1] = time;    
            CPU[0] = {-1, -1};        
        }

        if(rt2 == 0 && p2.second != -1)
        {
            indices[ind2]++;
            if(data[ind2][indices[ind2]] != -1)
                wait[data[ind2][indices[ind2]] + time].push_back(ind2);
            else
                details[ind2][1] = time;
            CPU[1] = {-1, -1};
        }

        if(pq.size() > 0)
        {
            // Assign to CPU0
            pair<int,int> top1 = pq.top();
            if(rt1 > top1.first || rt1 <= 0)
            {
                pq.pop();
                CPU[0] = {top1.first, top1.second};
                if(rt1 > 0)
                {
                    pq.push({rt1, ind1});
                }
            }
                
            // Assign to CPU1
            if(pq.size() > 0)
            {
                pair<int,int> top2 = pq.top();
                if(rt2 > top2.first || rt2 <= 0)
                {
                    pq.pop();
                    CPU[1] = {top2.first, top2.second};
                    if(rt2 > 0)
                        pq.push({rt2, ind2});                       
                }
            }

            // Update CPU0 statistics
            if (current_cpu0.first != CPU[0].second) {
                // Close previous CPU0 execution
                if (current_cpu0.first != -1) {
                    cpu0_stats.back().end_time = time;
                }
                // Assign new process to CPU0
                if (CPU[0].second != -1) {
                    process_bursts[CPU[0].second]++;
                    CPUStat new_stat = {CPU[0].second, process_bursts[CPU[0].second], time, -1};
                    cpu0_stats.push_back(new_stat);
                }
                current_cpu0.first = CPU[0].second;
                current_cpu0.second = time;
            }

            // Update CPU1 statistics
            if (current_cpu1.first != CPU[1].second) {
                // Close previous CPU1 execution
                if (current_cpu1.first != -1) {
                    cpu1_stats.back().end_time = time;
                }
                // Assign new process to CPU1
                if (CPU[1].second != -1) {
                    process_bursts[CPU[1].second]++;
                    CPUStat new_stat = {CPU[1].second, process_bursts[CPU[1].second], time, -1};
                    cpu1_stats.push_back(new_stat);
                }
                current_cpu1.first = CPU[1].second;
                current_cpu1.second = time;
            }
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

int main() {
    ifstream inputFile("process1.dat");
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
            vector<int> parsedLine = parseLine(line);
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
    vector<vector<int>> details = premsjf(names, data, cpu0_stats, cpu1_stats);

    // Output process details
    int noproc = data.size();
    

    // Output CPU0 statistics

    for (int i = 0; i < noproc; i++) {
        details[i][3] = details[i][1] - details[i][0];  // Makespan = CT - AT
        details[i][4] = details[i][3] - details[i][2];  // Waiting Time = MK - RT
    }
    cout << "CPU0" << endl;
    for(auto &stat : cpu0_stats){
        cout << "P" << stat.pid + 1 << "," << stat.burst << "\t" 
             << stat.start_time << "\t" << stat.end_time << endl;
    }

    // Output CPU1 statistics
    cout << "CPU1" << endl;
    for(auto &stat : cpu1_stats){
        cout << "P" << stat.pid + 1 << "," << stat.burst << "\t" 
             << stat.start_time << "\t" << stat.end_time << endl;
    }
    cout << "AT\tCT\tRT\tMK\tWT" << endl;
    for (int i = 0; i < noproc; i++) {
        for (int j = 0; j < 5; j++) {
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }

    return 0;
}