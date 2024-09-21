#ifndef PSJF2CPU_H
#define PSJF2CPU_H
#include <bits/stdc++.h>
using namespace std;
// Function declaration

struct CPUStat {
    int pid;        // Process ID
    int burst;      // Current burst number
    int start_time; // Start time of the burst
    int end_time;   // End time of the burst
};
void runPSJF2Cpu(const std::string& workloadFile);
vector<vector<int>> premsjf2(map<int, string>& names, vector<vector<int>>& data,
                            vector<CPUStat>& cpu0_stats, vector<CPUStat>& cpu1_stats);

#endif // MAIN_H