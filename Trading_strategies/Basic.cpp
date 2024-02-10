#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>

using namespace std;

int n = 5;             // previous days we want to check
int x = 2;             // limiting the position
int curr_position = 0; // mod(curr_position) <= x
double cumulative_cashflow = 0;

string date_formatting(string date){
    string s = "";
    s += date.substr(8,2);
    s+= "/";
    s += date.substr(5,2);
    s+= "/";
    s += date.substr(0,4);
    return s;
}

void readCSV(const string &filePath, vector<vector<string>> &data)
{ // reads the csv file
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
{ // create the csv file
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

    cout << "CSV file '" << filename << "' created successfully." << endl; // will remove this
}

string make_zero(int n)
{
    string s = "";
    for (int i = 0; i < n; i++)
    {
        s += '0';
    }
    return s;
}

void basic_strategy(const vector<vector<string>> &data, vector<vector<string>> &order_stats, vector<vector<string>> &CashFlow)
{
    string s = make_zero(n);
    int data_length = data.size() - 1; // the last element is the column name so subtracting 1 from it
    int i = n;
    while (i < data_length)
    {
        if (data[i][1] == data[i - 1][1])
        {
            // cout << date_formatting(data[i][0]) << " " << data[i][1] << " " << data[i - 1][1] << endl;
            
            for (int k = 0; k < n; k++)
            {
                if (i + k >= data_length)
                {
                    break;
                }
                CashFlow.push_back({date_formatting(data[i + k][0]), to_string(cumulative_cashflow)});
            }
            i += n;
        }
        else if (stod(data[i][1]) > stod(data[i - 1][1]))
        { // for checking if price is increased in n days or not
            int check = i;
            for (int j = 1; j < n; j++)
            {
                if (!(stod(data[i - j][1]) > stod(data[i - j - 1][1])))
                {

                    for (int k = 0; k < n - j; k++)
                    {
                        if (i + k >= data_length)
                        {
                            break;
                        }
                        CashFlow.push_back({date_formatting(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n - j;
                    break;
                }
            }
            if (check == i)
            {
                if (x == 2)
                { // we can't buy the stock because we are at the maximum limit

                    for (int k = 0; k < n; k++)
                    {
                        if (i + k >= data_length)
                        {
                            break;
                        }
                        CashFlow.push_back({date_formatting(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n; // since we can't buy stocks all positive increments are useless
                }
                else
                {
                    x++;
                    vector<string> order_stats_row;
                    vector<string> CashFlow_row;
                    string date = date_formatting(data[i][0]);
                    cumulative_cashflow -= stod(data[i][1]);
                    order_stats_row.push_back(date);
                    order_stats_row.push_back("BUY");
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);
                    CashFlow_row.push_back(date);
                    CashFlow_row.push_back(to_string(cumulative_cashflow));
                    CashFlow.push_back(CashFlow_row);
                    order_stats.push_back(order_stats_row);
                }
                i++;
            }
            
        }
        else
        { // for checking if price has decreased for n days or not
            int check = i;
            for (int j = 1; j < n; j++)
            {
                if (!(stod(data[i - j][1]) < stod(data[i - j - 1][1])))
                {
                    for (int k = 0; k < n - j; k++)
                    {
                        if (i + k >= data_length)
                        {
                            break;
                        }
                        CashFlow.push_back({date_formatting(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n - j;
                    break;
                }
            }
            if (check == i)
            {
                if (x == -2) // we can't buy the stock because we are at the maximum limit
                {

                    for (int k = 0; k < n; k++)
                    {
                        if (i + k >= data_length)
                        {
                            break;
                        }
                        CashFlow.push_back({date_formatting(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n; // since we can't buy stocks all positive increments are useless
                }
                else
                {
                    x--;
                    vector<string> order_stats_row;
                    vector<string> CashFlow_row;
                    string date = date_formatting(data[i][0]);
                    cumulative_cashflow += stod(data[i][1]);
                    order_stats_row.push_back(date);
                    order_stats_row.push_back("SELL");
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);
                    CashFlow_row.push_back(date);
                    CashFlow_row.push_back(to_string(cumulative_cashflow));
                    CashFlow.push_back(CashFlow_row);
                    order_stats.push_back(order_stats_row);
                }
                i++;
            }
        }
    }
}

int main()
{
    vector<vector<string>> data;
    readCSV("SBIN.csv", data);
    reverse(data.begin(), data.end());
    vector<vector<string>> CashFlow;
    vector<vector<string>> order_stats;
    order_stats.push_back({"Date","Order_dir","Quantity","Price"});
    CashFlow.push_back({"Date","CashFlow"});
    basic_strategy(data, order_stats, CashFlow);
    writeCSV("OrderStatistics.csv", order_stats);
    writeCSV("CashFlow.csv", CashFlow);
    return 0;
}