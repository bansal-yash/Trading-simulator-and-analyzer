#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cstdio>
#include <omp.h>

#include "basic.cpp"
#include "dma.cpp"
#include "dmapp.cpp"
#include "macd.cpp"
#include "rsi.cpp"
#include "adx.cpp"
#include "linear_regression.cpp"
#include "pairs.cpp"
#include "stop_loss_pairs.cpp"

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

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date + " basic";
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_basic.csv", data);
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

        FILE *file = fopen((symbol + "_basic.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_basic.csv").c_str());
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

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date + " dma";
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_dma.csv", data);
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

        FILE *file = fopen((symbol + "_dma.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_dma.csv").c_str());
        }
    }

    else if (strategy == "DMA++") /////////////////////////////////////////////////////////////////
    {
        string symbol = argv[2];
        string n = argv[3];
        string x = argv[4];
        string p = argv[5];
        string max_hold_days = argv[6];
        string c1 = argv[7];
        string c2 = argv[8];
        string start_date = argv[9];
        string end_date = argv[10];

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date + " dmapp";
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_dmapp.csv", data);
        reverse(data.begin(), data.end());

        double pnl = dmapp_strategy(data, CashFlow, order_stats, stoi(n), stoi(x), stod(p), stoi(max_hold_days), stod(c1), stod(c2));

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + "_dmapp.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_dmapp.csv").c_str());
        }
    }

    else if (strategy == "MACD")
    {
        string symbol = argv[2];
        string x = argv[3];
        string start_date = argv[4];
        string end_date = argv[5];

        string to_invoke = "python3 get_data.py " + symbol + " 0 " + start_date + " " + end_date + " macd";
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_macd.csv", data);
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

        FILE *file = fopen((symbol + "_macd.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_macd.csv").c_str());
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

        string to_invoke = "python3 get_data.py " + symbol + " " + n + " " + start_date + " " + end_date + " rsi";
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_rsi.csv", data);
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

        FILE *file = fopen((symbol + "_rsi.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_rsi.csv").c_str());
        }
    }

    else if (strategy == "ADX")
    {
        string symbol = argv[2];
        string n = argv[3];
        string x = argv[4];
        string adx_threshold = argv[5];
        string start_date = argv[6];
        string end_date = argv[7];

        string to_invoke = "python3 get_data_adx.py " + symbol + " 1 " + start_date + " " + end_date;
        system(to_invoke.c_str());

        vector<vector<string>> data;
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_adx.csv", data);
        reverse(data.begin(), data.end());

        double pnl = ADX_strategy(data, CashFlow, order_stats, stoi(x), stoi(n), stod(adx_threshold));

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + "_adx.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_adx.csv").c_str());
        }
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
        readCSV(symbol + "_linear.csv", data);
        reverse(data.begin(), data.end());
        data.pop_back();

        vector<double> coefficients;
        train_data(data, coefficients);

        to_invoke = "python3 get_data_linear.py " + symbol + " 1 " + start_date + " " + end_date;
        system(to_invoke.c_str());

        data = {};
        vector<vector<string>> order_stats;
        vector<vector<string>> CashFlow;

        readCSV(symbol + "_linear.csv", data);
        reverse(data.begin(), data.end());
        double pnl = linear_regression_strategy(data, coefficients, stoi(x), stod(p), order_stats, CashFlow);

        writeCSV("order_statistics.csv", order_stats);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file = fopen((symbol + "_linear.csv").c_str(), "r");
        if (file)
        {
            fclose(file);
            remove((symbol + "_linear.csv").c_str());
        }
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

        string to_invoke = "python3 get_data.py " + symbol1 + " " + n + " " + start_date + " " + end_date + " pairs";
        system(to_invoke.c_str());

        to_invoke = "python3 get_data.py " + symbol2 + " " + n + " " + start_date + " " + end_date + " pairs";
        system(to_invoke.c_str());

        vector<vector<string>> data1;
        vector<vector<string>> data2;
        vector<vector<string>> order_stats1;
        vector<vector<string>> order_stats2;
        vector<vector<string>> CashFlow;

        readCSV(symbol1 + "_pairs.csv", data1);
        reverse(data1.begin(), data1.end());

        readCSV(symbol2 + "_pairs.csv", data2);
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

        FILE *file1 = fopen((symbol1 + "_pairs.csv").c_str(), "r");
        if (file1)
        {
            fclose(file1);
            remove((symbol1 + "_pairs.csv").c_str());
        }

        FILE *file2 = fopen((symbol2 + "_pairs.csv").c_str(), "r");
        if (file2)
        {
            fclose(file2);
            remove((symbol2 + "_pairs.csv").c_str());
        }
    }

    else if (strategy == "PAIRS" && argc == 10) ////////////////////////////////////////////////////////////////////////////
    {
        string n = argv[2];
        string x = argv[3];
        string symbol1 = argv[4];
        string symbol2 = argv[5];
        string threshold = argv[6];
        string stop_loss_threshold = argv[7];
        string start_date = argv[8];
        string end_date = argv[9];

        string to_invoke = "python3 get_data.py " + symbol1 + " " + n + " " + start_date + " " + end_date + " pairs2";
        system(to_invoke.c_str());

        to_invoke = "python3 get_data.py " + symbol2 + " " + n + " " + start_date + " " + end_date + " pairs2";
        system(to_invoke.c_str());

        vector<vector<string>> data1;
        vector<vector<string>> data2;
        vector<vector<string>> order_stats1;
        vector<vector<string>> order_stats2;
        vector<vector<string>> CashFlow;

        readCSV(symbol1 + "_pairs2.csv", data1);
        reverse(data1.begin(), data1.end());

        readCSV(symbol2 + "_pairs2.csv", data2);
        reverse(data2.begin(), data2.end());

        double pnl = 0; ///////////////////////////////////////////////////////////////////////////////////////////////

        writeCSV("order_statistics_1.csv", order_stats1);
        writeCSV("order_statistics_2.csv", order_stats2);
        writeCSV("daily_cashflow.csv", CashFlow);
        ofstream outfile("final_pnl.txt", ios::app);
        if (outfile.is_open())
        {
            outfile << pnl << std::endl;
            outfile.close();
        }

        FILE *file1 = fopen((symbol1 + "_pairs2.csv").c_str(), "r");
        if (file1)
        {
            fclose(file1);
            remove((symbol1 + "_pairs2.csv").c_str());
        }

        FILE *file2 = fopen((symbol2 + "_pairs2.csv").c_str(), "r");
        if (file2)
        {
            fclose(file2);
            remove((symbol2 + "_pairs2.csv").c_str());
        }
    }

    else if (strategy == "BEST_OF_ALL")
    {
        string symbol = argv[2];
        string start_date = argv[3];
        string end_date = argv[4];

        vector<vector<string>> data_basic;
        vector<vector<string>> order_stats_basic;
        vector<vector<string>> CashFlow_basic;
        double pnl_basic;

        vector<vector<string>> data_dma;
        vector<vector<string>> order_stats_dma;
        vector<vector<string>> CashFlow_dma;
        double pnl_dma;

        vector<vector<string>> data_dmapp;
        vector<vector<string>> order_stats_dmapp;
        vector<vector<string>> CashFlow_dmapp;
        double pnl_dmapp;

        vector<vector<string>> data_macd;
        vector<vector<string>> order_stats_macd;
        vector<vector<string>> CashFlow_macd;
        double pnl_macd;

        vector<vector<string>> data_rsi;
        vector<vector<string>> order_stats_rsi;
        vector<vector<string>> CashFlow_rsi;
        double pnl_rsi;

        vector<vector<string>> data_adx;
        vector<vector<string>> order_stats_adx;
        vector<vector<string>> CashFlow_adx;
        double pnl_adx;

        vector<vector<string>> data_linear;
        vector<double> coefficients_linear;
        vector<vector<string>> order_stats_linear;
        vector<vector<string>> CashFlow_linear;
        double pnl_linear;

#pragma omp parallel sections
        {
#pragma omp section // basic
            {string to_invoke = "python3 get_data.py " + symbol + " 7 " + start_date + " " + end_date + " basic";
        system(to_invoke.c_str());
    }

#pragma omp section // dma
    {
        string to_invoke = "python3 get_data.py " + symbol + " 50 " + start_date + " " + end_date + " dma";
        system(to_invoke.c_str());
    }

#pragma omp section // dma++
    {
        string to_invoke = "python3 get_data.py " + symbol + " 14 " + start_date + " " + end_date + " dmapp";
        system(to_invoke.c_str());
    }

#pragma omp section // macd
    {
        string to_invoke = "python3 get_data.py " + symbol + " 0 " + start_date + " " + end_date + " macd";
        system(to_invoke.c_str());
    }

#pragma omp section // rsi
    {
        string to_invoke = "python3 get_data.py " + symbol + " 14 " + start_date + " " + end_date + " rsi";
        system(to_invoke.c_str());
    }

#pragma omp section // adx
    {
        string to_invoke = "python3 get_data_adx.py " + symbol + " 1 " + start_date + " " + end_date;
        system(to_invoke.c_str());
    }

#pragma omp section // linear regresiion
    {
        int c = stoi(start_date.substr(6, 4)) - 1;
        string train_start_date = start_date.substr(0, 6) + to_string(c);
        c = stoi(end_date.substr(6, 4)) - 1;
        string train_end_date = end_date.substr(0, 6) + to_string(c);

        string to_invoke = "python3 get_data_linear.py " + symbol + " 1 " + train_start_date + " " + train_end_date;
        system(to_invoke.c_str());

        readCSV(symbol + "_linear.csv", data_linear);
        reverse(data_linear.begin(), data_linear.end());
        data_linear.pop_back();

        train_data(data_linear, coefficients_linear);
        data_linear = {};

        to_invoke = "python3 get_data_linear.py " + symbol + " 1 " + start_date + " " + end_date;
        system(to_invoke.c_str());
    }
}

#pragma omp barrier

#pragma omp parallel sections
{
#pragma omp section // basic
    {
        readCSV(symbol + "_basic.csv", data_basic);
        reverse(data_basic.begin(), data_basic.end());
        pnl_basic = basic_strategy(data_basic, 7, 5, order_stats_basic, CashFlow_basic);
    }

#pragma omp section // dma
    {
        readCSV(symbol + "_dma.csv", data_dma);
        reverse(data_dma.begin(), data_dma.end());
        pnl_dma = DMA_strategy(data_dma, 50, 5, stod("2"), order_stats_dma, CashFlow_dma);
    }

#pragma omp section // dma++
    {
        readCSV(symbol + "_dmapp.csv", data_dmapp);
        reverse(data_dmapp.begin(), data_dmapp.end());
        pnl_dmapp = dmapp_strategy(data_dmapp, CashFlow_dmapp, order_stats_dmapp, 14, 5, stod("5"), 28, stod("2"), stod("0.2"));
    }

#pragma omp section // macd
    {
        readCSV(symbol + "_macd.csv", data_macd);
        reverse(data_macd.begin(), data_macd.end());
        pnl_macd = MACD_strategy(data_macd, CashFlow_macd, order_stats_macd, 5);
    }

#pragma omp section // rsi
    {
        readCSV(symbol + "_rsi.csv", data_rsi);
        reverse(data_rsi.begin(), data_rsi.end());
        pnl_rsi = RSI_strategy(data_rsi, CashFlow_rsi, order_stats_rsi, 14, 5, stod("30"), stod("70"));
    }

#pragma omp section // adx
    {
        readCSV(symbol + "_adx.csv", data_adx);
        reverse(data_adx.begin(), data_adx.end());
        pnl_adx = ADX_strategy(data_adx, CashFlow_adx, order_stats_adx, 5, 14, stod("25"));
    }

#pragma omp section // linear regresiion
    {
        readCSV(symbol + "_linear.csv", data_linear);
        reverse(data_linear.begin(), data_linear.end());
        pnl_linear = linear_regression_strategy(data_linear, coefficients_linear, 5, stod("2.0"), order_stats_linear, CashFlow_linear);
    }
}

#pragma barrier

// cout << "pnl_basic :- " << pnl_basic << endl;
// cout << "pnl_dma :- " << pnl_dma << endl;
// cout << "pnl_dmapp :- " << pnl_dmapp << endl;
// cout << "pnl_macd :- " << pnl_macd << endl;
// cout << "pnl_rsi :- " << pnl_rsi << endl;
// cout << "pnl_adx :- " << pnl_adx << endl;
// cout << "pnl_linear :- " << pnl_linear << endl;

string best_strategy = "basic";
double final_pnl = pnl_basic;
if (final_pnl <= pnl_dma)
{
    best_strategy = "dma";
    final_pnl = pnl_dma;
}
if (final_pnl <= pnl_dmapp)
{
    best_strategy = "dmapp";
    final_pnl = pnl_dmapp;
}
if (final_pnl <= pnl_macd)
{
    best_strategy = "macd";
    final_pnl = pnl_macd;
}
if (final_pnl <= pnl_rsi)
{
    best_strategy = "rsi";
    final_pnl = pnl_rsi;
}
if (final_pnl <= pnl_adx)
{
    best_strategy = "adx";
    final_pnl = pnl_adx;
}
if (final_pnl <= pnl_linear)
{
    best_strategy = "linear";
    final_pnl = pnl_linear;
}

if (best_strategy == "basic")
{
    writeCSV("order_statistics.csv", order_stats_basic);
    writeCSV("daily_cashflow.csv", CashFlow_basic);
}
else if (best_strategy == "dma")
{
    writeCSV("order_statistics.csv", order_stats_dma);
    writeCSV("daily_cashflow.csv", CashFlow_dma);
}
else if (best_strategy == "dmapp")
{
    writeCSV("order_statistics.csv", order_stats_dmapp);
    writeCSV("daily_cashflow.csv", CashFlow_dmapp);
}
else if (best_strategy == "macd")
{
    writeCSV("order_statistics.csv", order_stats_macd);
    writeCSV("daily_cashflow.csv", CashFlow_macd);
}
else if (best_strategy == "rsi")
{
    writeCSV("order_statistics.csv", order_stats_rsi);
    writeCSV("daily_cashflow.csv", CashFlow_rsi);
}
else if (best_strategy == "adx")
{
    writeCSV("order_statistics.csv", order_stats_adx);
    writeCSV("daily_cashflow.csv", CashFlow_adx);
}
else if (best_strategy == "linear")
{
    writeCSV("order_statistics.csv", order_stats_linear);
    writeCSV("daily_cashflow.csv", CashFlow_linear);
}

ofstream outfile("final_pnl.txt", ios::app);
if (outfile.is_open())
{
    outfile << final_pnl << std::endl;
    outfile.close();
}

string arr[7] = {"basic", "dma", "dmapp", "macd", "rsi", "adx", "linear"};
for (string word : arr)
{
    FILE *file = fopen((symbol + "_" + word + ".csv").c_str(), "r");
    if (file)
    {
        fclose(file);
        remove((symbol + "_" + word + ".csv").c_str());
    }
}
}
}
