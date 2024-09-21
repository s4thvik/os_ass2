#include <bits/stdc++.h>
#include "parseLine1.h"
using namespace std;

vector<int> parseLine1(const string& line) {
    vector<int> result;
    stringstream ss(line);
    int number;
    while (ss >> number) {
        result.push_back(number);
    }
    return result;
}