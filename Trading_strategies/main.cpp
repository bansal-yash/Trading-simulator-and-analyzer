#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>

#include "basic.cpp"
#include "dma.cpp"
#include "linear_regression.cpp"
#include "macd.cpp"
#include "rsi.cpp"
#include "pairs.cpp"

using namespace std;

void readCSV(const string &filePath, vector<vector<string>> &data)
{
    ifstream file(filePath);
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
        double pnl = basic_strategy(data, stoi(n), stoi(x), order_stats, CashFlow);

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + ".csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + ".csv").c_str());
        }
    }

    else if (strategy == "DMA")
    {
        string symbol = argv[2];
        string n = argv[3];
        string x = argv[4];
        string p = argv[5];
        string start_date = argv[6];
        string end_date = argv[7];

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        double pnl = DMA_strategy(data, stoi(n), stoi(x), stod(p), order_stats, CashFlow);

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + ".csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + ".csv").c_str());
        }
    }

    else if (strategy == "DMA++")
    {
        int x = 1;
    }

    else if (strategy == "MACD")
    {
        string symbol = argv[2];
        string x = argv[3];
        string start_date = argv[4];
        string end_date = argv[5];

        string to_invoke = "python3 get_data.py " + symbol + " 0 " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        double pnl = MACD_strategy(data, CashFlow, order_stats, stoi(x));

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + ".csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + ".csv").c_str());
        }
    }

    else if (strategy == "RSI")
    {
        string symbol = argv[2];
        string n = argv[3];
        string x = argv[4];
        string oversold_threshold = argv[5];
        string overbought_threshold = argv[6];
        string start_date = argv[7];
        string end_date = argv[8];

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        double pnl = RSI_strategy(data, CashFlow, order_stats, stoi(n), stoi(x), stod(oversold_threshold), stod(overbought_threshold));

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + ".csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + ".csv").c_str());
        }
    }

    else if (strategy == "ADX")
    {
        int x = 1;
    }

    else if (strategy == "LINEAR_REGRESSION")
    {
        string symbol = argv[2];
        string x = argv[3];
        string p = argv[4];
        string train_start_date = argv[5];
        string train_end_date = argv[6];
        string start_date = argv[7];
        string end_date = argv[8];

        string to_invoke = "python3 get_data_linear.py " + symbol + " 1 " + train_start_date + " " + train_end_date;
        system(to_invoke.c_str());
        vector<vector<string>> data;
        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        data.pop_back();

        vector<double> coefficients;
        train_data(data, coefficients);

        to_invoke = "python3 get_data_linear.py " + symbol + " 1 " + start_date + " " + end_date;
        system(to_invoke.c_str());

        data = {};
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + ".csv", data);
        reverse(data.begin(), data.end());
        double pnl = linear_regression_strategy(data, coefficients, stoi(x), stoi(p), order_stats, CashFlow);

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        system(("rm " + symbol + ".csv").c_str());
    }

    else if (strategy == "BEST_OF_ALL")
    {
        int x = 1;
    }

    else if (strategy == "PAIRS" && argc == 9)
    {
        string n = argv[2];
        string x = argv[3];
        string symbol1 = argv[4];
        string symbol2 = argv[5];
        string threshold = argv[6];
        string start_date = argv[7];
        string end_date = argv[8];

        string to_invoke = "python3 get_data.py " + symbol1 + " " + n + " " + start_date + " " + end_date;
        system(to_invoke.c_str());

        to_invoke = "python3 get_data.py " + symbol2 + " " + n + " " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data1;
        vector<vector<string>> data2;
        vector<vector<string>> order_stats1;
        vector<vector<string>> order_stats2;
        vector<vector<string>> CashFlow;

        readCSV(symbol1 + ".csv", data1);
        reverse(data1.begin(), data1.end());

        readCSV(symbol2 + ".csv", data2);
        reverse(data2.begin(), data2.end());

        double pnl = Pairs_strategy(data1, data2, CashFlow, order_stats1, order_stats2, stoi(x), stoi(n), stod(threshold));

        writeCSV("order_statistics_1.csv", order_stats1);
        writeCSV("order_statistics_2.csv", order_stats2);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file1 = fopen((symbol1 + ".csv").c_str(), "r");
        if (file1)
        {
            fclose(file1);
            remove((symbol1 + ".csv").c_str());
        }

        FILE *file2 = fopen((symbol2 + ".csv").c_str(), "r");
        if (file2)
        {
            fclose(file2);
            remove((symbol2 + ".csv").c_str());
        }
    }

    else if (strategy == "PAIRS" && argc == 10)
    {
        int x = 1;
        cout << "aaa" << endl;
    }
}
