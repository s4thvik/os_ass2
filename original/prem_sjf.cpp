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


void premsjf(map<int,string> &names, vector<vector<int>> &data)
{
    int noproc = data.size();
    vector<vector<int>> details(noproc,vector<int>(6,0));
    int time=0;

    priority_queue<pair<int,int>, vector<pair<int,int>>, greater<pair<int,int>>> pq;
    deque<pair<int,int>> wait;
    vector<int> indices(noproc,0);
    int allarr=0;
    indices[0] = 1;
    details[0][0] = 0;
    details[0][2] = data[0][1];
    pq.push({data[0][1],0});
    
    while(pq.size()!=0 || wait.size()!=0 || allarr==0)
    {
        time++;
        if(wait.size()>0)
        {
            pair<int,int> w = wait.front();
            int io = w.first;
            int indi = w.second;
            wait.pop_front();
        
            if(io-1>0)
                wait.push_front({io-1,indi});
            else
            {
                indices[indi]+=1;
                if(data[indi][indices[indi]]!=-1)
                {
                    pq.push({data[indi][indices[indi]],indi});
                    details[indi][2]+=data[indi][indices[indi]];
                }
                else
                {
                    details[indi][1] = time;               
                }
            }
        }


        if(pq.size()>0)
        {
            pair<int,int> p = pq.top();
            pq.pop();
            int rt = p.first;
            int ind = p.second;

            if(rt-1>0)
                pq.push({rt-1,ind});
            else
            {
                indices[ind]+=1;
                if(data[ind][indices[ind]]!=-1)
                {
                    wait.push_back({data[ind][indices[ind]],ind});
                    details[ind][5]=data[ind][indices[ind]];
                }
                else
                {
                    details[ind][1] = time;
                }
            }
        }
        if(allarr==0)
        {
            int t=1;
            for(int i=0;i<noproc;i++)
            {
                if(time==data[i][0])
                {
                    pq.push({data[i][1],i});
                    indices[i] = 1;
                    details[i][0] = time;
                    details[i][2]+=data[i][1];
                }
                t = min(t,indices[i]);
            }
            if(t>0)
                allarr=1;
        }
        

    }

    for(int i=0;i<noproc;i++)
    {
        details[i][3] = (details[i][1]-details[i][0]);
        details[i][4] = (details[i][3]-details[i][2]);        
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
    premsjf(names,data);

    return 0;
}