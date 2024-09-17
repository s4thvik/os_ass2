#include<bits/stdc++.h>

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


void fifo(map<int,string> &names, vector<vector<int>> &data)
{
    int noproc = data.size();
    vector<vector<int>> details;
    int time=0;

    for(int i=0;i<noproc;i++)
    {
        vector<int> temp(6,0);
        temp[0] = data[i][0];
        if(time<temp[0])
            time=temp[0];
        int k = data[i][1];
        int j = 1;
        while(k!=-1)
        {
            time+=k;
            if(j%2)
                temp[2]+=k;
            else
                temp[5]+=k;
            j++;
            k = data[i][j];
        }

        temp[1] = time;
        temp[3] = (temp[1]-temp[0]);
        temp[4] = (temp[3]-temp[2]);

        details.push_back(temp);
    }

    cout << "AT CT RT MK WT" << endl; 
    for(int i=0;i<noproc;i++)
    {
        for(int j=0;j<5;j++)
        {
            cout << details[i][j] << " ";
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

    // for (const auto& row : data) {
    //     for (int num : row) {
    //         cout << num << " ";
    //     }
    //     cout << endl;
    // }


    map<int,string> names;
    names.insert({0,"Arrival Time"});
    names.insert({1,"Completion Time"});
    names.insert({2,"Run Time"});
    names.insert({3,"Makespan"});
    names.insert({4,"Waiting Time"});
    names.insert({5,"I/O Time"});

    fifo(names,data);

    return 0;
}