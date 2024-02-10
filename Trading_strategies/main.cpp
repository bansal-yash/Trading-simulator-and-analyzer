#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdlib>

#include "basic.cpp"
#include "dma.cpp"

using namespace std;

void readCSV(const string &filePath, vector<vector<string>> &data)
{
    ifstream file(filePath);
    if (!file.is_open())
    {
        cerr << "Error opening file!" << endl;
        return;
    }
    string line;
    while (getline(file, line))
    {
        stringstream ss(line);
        string cell;
        vector<string> row;
        while (getline(ss, cell, ','))
        {
            row.push_back(cell);
        }

        data.push_back(row);
    }
    file.close();
}

void writeCSV(const string &filename, const vector<vector<string>> &data)
{
    ofstream file(filename);
    if (!file.is_open())
    {
        cerr << "Error opening file!" << endl;
        return;
    }
    for (const auto &row : data)
    {
        for (auto iter = row.begin(); iter != row.end(); ++iter)
        {
            file << *iter;
            if (iter != row.end() - 1)
            {
                file << ",";
            }
        }
        file << endl;
    }
    file.close();
}

int main(int argc, char *argv[])
{
    string strategy = argv[1];

    if (strategy == "BASIC")
    {
        string symbol = argv[2];
        string n = argv[3];
        string x = argv[4];
        string start_date = argv[5];
        string end_date = argv[6];

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        basic_strategy(data, stoi(n), stoi(x), order_stats, CashFlow);

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);

        system(("rm " + symbol + ".csv").c_str());
    }

    else if (strategy == "DMA")
    {
        string symbol = argv[2];
        string n = argv[3];
        string x = argv[4];
        string p = argv[5];
        string start_date = argv[6];
        string end_date = argv[7];

        cout << strategy << " " << symbol << " " << n << " " << x << " " << p << " " << start_date << " " << end_date << endl;
        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        DMA_strategy(data, stoi(n), stoi(x), stoi(p), order_stats, CashFlow);

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);

        system(("rm " + symbol + ".csv").c_str());
    }
}
