#include<bits/stdc++.h>
#include "parseLine1.h"

using namespace std;
vector<int> parseLine1(const std::string& line);


void premsjf1(map<int,string> &names, vector<vector<int>> &data)
{
    int noproc = data.size();
    vector<vector<int>> details(noproc,vector<int>(6,0));
    int time = 0;

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    deque<pair<int,int>> wait;
    vector<int> indices(noproc, 0);  // To track the current position in the data array for each process
    int allarr = 0;
    indices[0] = 1;
    details[0][0] = 0;
    details[0][2] = data[0][1];
    pq.push({data[0][1], 0});
    
    // To store process execution details (process number, burst number, start time, end time)
    vector<tuple<int, int, int, int>> processDetails;

    int currentProcess = -1;  // Track the current process ID
    int startTime = -1;       // Track the start time of the current process burst

    while(pq.size() != 0 || wait.size() != 0 || allarr == 0)
    {
        // Handling I/O wait
        if(wait.size() > 0)
        {
            pair<int,int> w = wait.front();
            int io = w.first;
            int indi = w.second;
            wait.pop_front();
        
            if(io-1 > 0)
                wait.push_front({io-1, indi});
            else
            {
                indices[indi] += 1;
                if(data[indi][indices[indi]] != -1)
                {
                    pq.push({data[indi][indices[indi]], indi});
                    details[indi][2] += data[indi][indices[indi]];
                }
                else
                {
                    details[indi][1] = time;  // Completion time of the process               
                }
            }
        }

        // Running the process from the priority queue
        if(pq.size() > 0)
        {
            pair<int,int> p = pq.top();
            pq.pop();
            int rt = p.first;
            int ind = p.second;

            if (ind != currentProcess) {
                // Context switch - record the previous process's details
                if (currentProcess != -1 && startTime != -1) {
                    int burstNum = (indices[currentProcess] + 1) / 2;
                    processDetails.push_back({currentProcess + 1, burstNum, startTime, time - 1});
                }
                // New process starts execution
                currentProcess = ind;
                startTime = time;
            }

            time++;  // Increment time by 1 unit

            if(rt-1 > 0)
                pq.push({rt-1, ind});
            else
            {
                indices[ind] += 1;
                if(data[ind][indices[ind]] != -1)
                {
                    wait.push_back({data[ind][indices[ind]], ind});
                    details[ind][5] = data[ind][indices[ind]];  // I/O Time
                }
                else
                {
                    details[ind][1] = time;  // Completion time of the process
                }
            }
        }

        // Adding new arriving processes
        if(allarr == 0)
        {
            int t = 1;
            for(int i = 0; i < noproc; i++)
            {
                if(time == data[i][0])
                {
                    pq.push({data[i][1], i});
                    indices[i] = 1;
                    details[i][0] = time;  // Arrival time
                    details[i][2] += data[i][1];  // Run time
                }
                t = min(t, indices[i]);
            }
            if(t > 0)
                allarr = 1;
        }
    }

    // Ensure the last process is also recorded when it finishes
    if (currentProcess != -1 && startTime != -1) {
        int burstNum = (indices[currentProcess] + 1) / 2;
        processDetails.push_back({currentProcess + 1, burstNum, startTime, time - 1});
    }

    // Calculate makespan and waiting time
    for(int i = 0; i < noproc; i++)
    {
        details[i][3] = (details[i][1] - details[i][0]);  // Makespan
        details[i][4] = (details[i][3] - details[i][2]);  // Waiting Time       
    }

    // Print process burst details when the CPU switches processes
    cout << "CPU0" << endl;
    for(const auto& pd : processDetails) {
        int prNo, burstNo, startTime, endTime;
        tie(prNo, burstNo, startTime, endTime) = pd;
        cout << "P" << prNo << "," << burstNo << "\t" << startTime << "\t" << endTime << endl;
    }

    cout << "--------------------" << endl;
    
    // Print AT, CT, RT, MK, WT details
    cout << "AT\tCT\tRT\tMK\tWT" << endl; 
    for(int i = 0; i < noproc; i++)
    {
        for(int j = 0; j < 5; j++)
        {   
            cout << details[i][j] << "\t";
        }
        cout << endl;
    }    
}

void runPSJF1Cpu(const std::string& workloadFile) {
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
    
    premsjf1(names, data);
}