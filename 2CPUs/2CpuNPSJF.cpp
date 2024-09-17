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

void premsjf(map<int, string>& names, vector<vector<int>>& data) {
    int noproc = data.size();
    vector<vector<int>> details(noproc, vector<int>(6, 0));  // process details: AT, CT, RT, MK, WT
    int time = 0;
    vector<pair<int, int>> CPU(2, {-1, -1});  // (remaining time, process id)
    priority_queue<pair<int, int>, vector<pair<int, int>>, greater<pair<int, int>>> pq;  // (remaining time, process id)
    map<int, vector<int>> wait;  // stores waiting processes with time as key
    int allarr = 0;
    vector<int> indices(noproc, 0);
    indices[0] = 1;
    CPU[0] = {data[0][1], 0};  // first process starts on CPU

    while (CPU[0].first != 0 || CPU[1].first != 0 || !pq.empty() || !wait.empty() || allarr == 0) {
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

        // When CPU time is 0, load the next process from the queue
        if (CPU[0].first <= 0 || CPU[1].first <= 0) {
            pair<int, int> p1 = CPU[0], p2 = CPU[1];
            int ind1 = p1.second, ind2 = p2.second;

            if (ind1 >= 0) {
                indices[ind1]++;
                if (indices[ind1] < data[ind1].size() && data[ind1][indices[ind1]] != -1)
                    wait[time + data[ind1][indices[ind1]]].push_back(ind1);  // Waiting for I/O
                else
                    details[ind1][1] = time;  // Completion time
            }

            if (ind2 >= 0) {
                indices[ind2]++;
                if (indices[ind2] < data[ind2].size() && data[ind2][indices[ind2]] != -1)
                    wait[time + data[ind2][indices[ind2]]].push_back(ind2);  // Waiting for I/O
                else
                    details[ind2][1] = time;  // Completion time
            }

            if (CPU[0].first <= 0 && !pq.empty()) {
                CPU[0] = pq.top();
                pq.pop();
            }
            if (CPU[1].first <= 0 && !pq.empty()) {
                CPU[1] = pq.top();
                pq.pop();
            }
        }
    }

    // Output results
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

    premsjf(names, data);

    return 0;
}