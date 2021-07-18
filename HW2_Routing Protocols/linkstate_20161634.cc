#include <bits/stdc++.h>
#define INF 1987654321
using namespace std;

//g++ -Wall -o linkstate_20161634 linkstate_20161634.cc
//g++ -Wall -o distvec_20161634 distvec_20161634.cc
//execute example: ./linkstate_20161634 topologyfile messagesfile changesfile
//execute example: ./distvec_20161634 topologyfile messagesfile changesfile

struct 

vector<vector<int> > g;
vector<vector<pair<int, int> > >tab;
int N;
ofstream output;



struct cmp{
    bool operator()(pair<int,int>&a, pair<int,int>&b){
        if(a.first==b.first){
            return a.second>b.second;
        }
        else{
            return a.first>b.first;
        }
    }
};

void message_task(char *line){
    int from,to;
    char message[100];
    vector<int> v;
    sscanf(line, "%d %d %[^\n]s", &from, &to, message);

    if(tab[from][to].second==INF){
        output<< "from " << from << " to " << to << " cost infinite hops unreachable message " << message << endl; 
        return;
    }
    output << "from " << from << " to " << to << " cost " << tab[from][to].second << " hops";
    for(int i=to;i!=from;i=tab[from][i].first){
        v.push_back(i);
    }
    reverse(v.begin(), v.end());
    output << " " << from;
    for(int i=0;i<(int)v.size()-1;i++) {
        output << " " << v[i];
    }
    output << " message " << message <<endl;
}

void print_RoutingTable(void) {
    for (int i = 0;i < N;i++) {

        for (int j = 0;j < N;j++) {
            
            if(tab[i][j].first==-1) continue;
            output << j << " ";

            int par=j, prev=i;
            for(;par!=i;par=tab[i][par].first){
                prev=par;
            };
            output << prev << " ";

            output << tab[i][j].second << endl;
        }
        output << endl;
    }
    return;
}


void link_state(int n){
    priority_queue<pair<int,int>,vector<pair<int,int> >, cmp> pq;

    pq.push(make_pair(0,n));

    while(!pq.empty()){
        int cost=pq.top().first;
        int cur=pq.top().second;
        pq.pop();
        if(tab[n][cur].second<cost){
            continue;
        }
        //tiebreaking?

        for(int i=0;i<N;i++){
            if(g[cur][i]!=INF){
                int nextNode=i;
                int nextDist=cost+g[cur][nextNode];
                if(tab[n][nextNode].second>nextDist){
                    tab[n][nextNode].second=nextDist;
                    tab[n][nextNode].first=cur;
                    pq.push(make_pair(nextDist, nextNode));
                }
            }
        }


    }

}

int main(int argc, char* argv[]) {

    if (argc != 4) {
        cout << "usage: distvec topologyfile messagefile changesfile" << endl;
        exit(1);
    }

    ifstream topologyfile(argv[1]);
    ifstream messagefile(argv[2]);
    ifstream changesfile(argv[3]);
    output.open("output_ls.txt");

    if (!topologyfile || !messagefile || !changesfile) {
        cout << "Error: open input file." << endl;
        exit(1);
    }


    topologyfile >> N;
    g.resize(N + 1);
    tab.resize(N + 1);


    //initiate routing tables
    for (int i = 0;i < N;i++) {
        g[i].resize(N + 1);
        tab[i].resize(N + 1);
        for (int j = 0;j < N;j++) {
            g[i][j] = INF;
        }
    }

    int s, e , cost;
    while(topologyfile >> s >> e >> cost){
        g[s][e] = cost;
        g[e][s] = cost;
    }

    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            if (i == j) {
                tab[i][j].first = i;
                tab[i][j].second = 0;
            }
            else {
                if (tab[i][j].second == 0) {
                    tab[i][j].first = -1;
                    tab[i][j].second = INF;
                }
            }
        }
    }
    char buffer[100];
    for(int i=0;i<N;i++) link_state(i);
    print_RoutingTable();

    while(messagefile.getline(buffer, sizeof(buffer))){
        message_task(buffer);
    }
    output << endl;

    while(changesfile >> s >> e >> cost){
        messagefile.clear();
        messagefile.seekg(0);

        for (int i = 0;i < N;i++) {
            for (int j = 0;j < N;j++) {
                tab[i][j].first=-1;
                tab[i][j].second=INF;
                if (i == j) {
                    tab[i][j].first = i;
                    tab[i][j].second = 0;
                }
            }
        }

        if(cost==-999) {
            g[s][e]=INF;
            g[e][s]=INF;
        }
        else {
            g[s][e]=cost;
            g[e][s]=cost;
        }
        for(int i=0;i<N;i++) 
            link_state(i);
        
        print_RoutingTable();
        while(messagefile.getline(buffer, sizeof(buffer))){
            message_task(buffer);
        }
        output << endl;
    }
    

    //IF PROGRAM SUCCEEDS
    cout << "Complete. Output file written to output_ls.txt" << endl;
    topologyfile.close();
    messagefile.close();
    changesfile.close();


}