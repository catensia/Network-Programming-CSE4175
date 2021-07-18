#include <bits/stdc++.h>
#define INF 1987654321
using namespace std;



vector<vector<int> > g;
vector<vector<pair<int, int> > >tab;
int N;
queue <pair<int,int> > q;
ofstream output;

void print_RoutingTable(void) {
    for (int i = 0;i < N;i++) {
        for (int j = 0;j < N;j++) {
            if(tab[i][j].first==-1) continue;
            output << j << " " << tab[i][j].first << " " << tab[i][j].second << endl;
        }
        output << endl;
    }
    return;
}

void exchange_table(int a, int b){
    for(int i=0;i<N;i++){
        if(i==a) continue;
        if(tab[a][i].second>g[a][b]+tab[b][i].second){
            tab[a][i].second=g[a][b]+tab[b][i].second;
            tab[a][i].first=b;
            for(int j=0;j<N;j++){
                if(g[a][j]!=INF){
                    q.push(make_pair(a,j));
                }
            }
        }
        else if(tab[a][i].second==g[a][b]+tab[b][i].second){
            //tie-breaking rule
            if(tab[a][i].first>b){
                tab[a][i].first=b;
            }
        }
    }
}

void update_BellmanFord(void) {

    for (int i = 0;i < N;i++) {
        //for all nodes
        for (int j = 0;j < N;j++) {
            if (g[i][j] != INF) {
                //get adjacent nodes
                exchange_table(i,j);
                exchange_table(j,i);
            }
        }
    }
    //scheduler is implemented by queue
    while(!q.empty()){
        int f=q.front().first;
        int s=q.front().second;
        q.pop();
        exchange_table(f,s);
        exchange_table(s,f);
    }

}

void message_task(char *line){
    int from,to;
    char message[100];
    sscanf(line, "%d %d %[^\n]s", &from, &to, message);

    if(tab[from][to].second==INF){
        output<< "from " << from << " to " << to << " cost infinite hops unreachable message " << message << endl; 
        return;
    }
    output << "from " << from << " to " << to << " cost " << tab[from][to].second << " hops";
    for(int i=from;i!=to;i=tab[i][to].first){
        output << " " << i;
    }
    output << " message " << message << endl;
}


int main(int argc, char* argv[]) {

    if (argc != 4) {
        cout << "usage: distvec topologyfile messagefile changesfile" << endl;
        exit(1);
    }

    ifstream topologyfile(argv[1]);
    ifstream messagefile(argv[2]);
    ifstream changesfile(argv[3]);
    output.open("output_dv.txt");

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

    int s, e, cost;
    while(topologyfile >> s >> e >> cost){
        g[s][e] = cost;
        g[e][s] = cost;
        tab[s][e].first = e; tab[s][e].second = cost;
        tab[e][s].first = s; tab[e][s].second = cost;
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

    update_BellmanFord();
    print_RoutingTable();
    while(messagefile.getline(buffer, sizeof(buffer))){
        message_task(buffer);
    }
    output<<"\n";

    while(changesfile >> s >> e >> cost){
        messagefile.clear();
        messagefile.seekg(0);
        if(cost==-999) {
            g[s][e]=INF;
            g[e][s]=INF;
            for(int i=0;i<N;i++){
                for(int j=0;j<N;j++){
                    if(i==j) continue;
                    if(i==s || i==e || j==s || j==e){
                        tab[i][j].first=tab[j][i].first=-1;
                        tab[i][j].second=tab[j][i].second=INF;
                    }
                    if(tab[i][j].first==s || tab[i][j].first==e){
                        tab[i][j].first=tab[j][i].first=-1;
                        tab[i][j].second=tab[j][i].second=INF;
                    }
                }
            }

        }
        else {
            g[s][e]=cost;
            g[e][s]=cost;
        }
        update_BellmanFord();
        print_RoutingTable();
        while(messagefile.getline(buffer, sizeof(buffer))){
            message_task(buffer);
        }
        output << "\n";
        

    }    



    //IF PROGRAM SUCCEEDS
    cout << "Complete. Output file written to output_dv.txt" << endl;
    topologyfile.close();
    messagefile.close();
    changesfile.close();


}