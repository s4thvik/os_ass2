#include <iostream>
#include <bits/stdc++.h>
#include <vector>
#include <string>
#include <sstream>  // Include this
#include <algorithm>  // For sorting
#include <map>  // For maps
#include <queue>  // For queues
#include "parseLine2.h"
using namespace std;

vector<int> parseLine2(const string& line) {
    vector<int> result;
    stringstream ss(line);
    int number;
    while (ss >> number) {
        result.push_back(number);
    }
    return result;
}