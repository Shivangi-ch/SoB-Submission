#include<bits/stdc++.h>
using namespace std;

// variables for raw data from mempool.csv
vector<int> fees;
vector<int> weight;
vector<vector<string>> txid_parents;
vector<string> transactions_ids;
vector<string> txids;

// variables to restructute data for algorithm implementation
unordered_map<string, pair<int, int>> txs;
unordered_map<string, vector<string>> parents;
unordered_map<string, pair<int, int>> real_txs;
unordered_map<string, vector<string>> real_parents;


vector<string> selected_block;

// weight of block/knapsack
int W = 4000000;

// read data form mempool.csv
void read_mempool()
{
    ifstream fin;
    fin.open("./mempool.csv");
    if(!fin.is_open()) throw std::runtime_error("Could not open file");

    string line, word;
    bool first_row = true;
    int i = 0;

    while(!fin.eof())
    {
        if(first_row == true)
        {
            fin>>line;
            line = "";
            first_row = false;
            continue;
        }
        fin>>line;
        vector<string> parents;
        int c = 0;
        for(char a: line)
        {
            if(a != ',' and a != ';')
            {
                word.push_back(a);
            }
            else if(a == ',')
            {
                if(c == 0)
                {
                    transactions_ids.push_back(word);
                }
                else if(c == 1)
                {
                    int miner_fee = stoi(word);
                    fees.push_back(miner_fee);
                }
                else if(c == 2)
                {
                    int tx_weight = stoi(word);
                    weight.push_back(tx_weight);
                }
                
                c += 1;
                word = "";
            }
            else if(a == ';')
            {
                parents.push_back(word);
                word = "";
            }
        }
        if(word != "")
        {
            parents.push_back(word);
            word = "";
        }
        txid_parents.push_back(parents);
        i += 1;
    }
}


// dfs for resolving fees and weight for parent dependency
void dfs(unordered_set<string> &vis, string id)
{
    if(vis.find(id) != vis.end())
    {
        return;
    }
    vis.insert(id);
    real_txs[id] = txs[id];
    real_parents[id] = parents[id];
    for(auto p: parents[id])
    {
        dfs(vis, p);
        real_txs[id].first += real_txs[p].first;
        real_txs[id].second += real_txs[p].second;
        for(int j = 0;j<real_parents[p].size(); j++)
        {
            real_parents[id].push_back(real_parents[p][j]);
        }
    }
}


// function to restructure data which we got form csv 
void structure_data()
{
    int n = transactions_ids.size();
    for(int i = 0;i<transactions_ids.size();i++)
    {
        txs[transactions_ids[i]] = {fees[i], weight[i]};
        parents[transactions_ids[i]] = txid_parents[i];
    }

    unordered_set<string> vis;
    for(int i = 0;i<n;i++)
    {
        if(vis.find(transactions_ids[i]) == vis.end())
        {
            dfs(vis, transactions_ids[i]);
        }
    }

}


// sorting (fees/weight) to select the transactions; Time complexity: O(nlogn)
void select_transactions_sorting()
{
    structure_data();

    vector<pair<double, string>> v;
    for(auto i: real_txs)
    {
        double density = (i.second.first * 1.00)/i.second.second;
        v.push_back({density, i.first});
    }
    sort(v.begin(), v.end());
    int ww = 0;
    unordered_set<string> m;
    for(int i = v.size()-1;i>=0;i--)
    {
        ww += real_txs[v[i].second].second;
        if(ww <= W)
        {
            selected_block.push_back(v[i].second);
            for(auto xx: real_parents[v[i].second])
            {
                if(m.find(xx) == m.end())
                {
                    selected_block.push_back(xx);
                    m.insert(xx);
                }
                
            }
        }
        else
        {
            ww -= real_txs[v[i].second].second;
        }
    }
    cout<<ww<<"\n";
}


// knapsack dynamic programming approach for selecting transactions; Time Complexity: O(nW)
void select_transactions_knapsack()
{

}


// writing output to block.txt
void write_block()
{
    ofstream fout;
    fout.open("./block.txt");
    cout<<selected_block.size()<<"\n";
    // cout<<"\n";
    for(int i = selected_block.size() - 1;i>=0;i--)
    {
        fout<<selected_block[i]<<"\n";
    }
}

int main()
{

    read_mempool();

    select_transactions_sorting();
    select_transactions_knapsack();

    write_block();
    return 0;
}


/*

*/