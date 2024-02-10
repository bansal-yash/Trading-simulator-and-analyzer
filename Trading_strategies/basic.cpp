#include <iostream>
#include <vector>

using namespace std;

string date_formatting_basic(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

void basic_strategy(const vector<vector<string>> &data, int n, int x, vector<vector<string>> &order_stats, vector<vector<string>> &CashFlow)
{
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;
    int i = n;
    int curr_pos = 0;

    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "CashFlow"});
    while (i < data_length)
    {
        if (data[i][1] == data[i - 1][1])
        {
            for (int k = 0; k < n; k++)
            {
                if (i + k >= data_length)
                {
                    break;
                }
                CashFlow.push_back({date_formatting_basic(data[i + k][0]), to_string(cumulative_cashflow)});
            }
            i += n;
        }
        else if (stod(data[i][1]) > stod(data[i - 1][1]))
        {
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
                        CashFlow.push_back({date_formatting_basic(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n - j;
                    break;
                }
            }
            if (check == i)
            {
                if (curr_pos == x)
                {
                    for (int k = 0; k < n; k++)
                    {
                        if (i + k >= data_length)
                        {
                            break;
                        }
                        CashFlow.push_back({date_formatting_basic(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n;
                }
                else
                {
                    curr_pos++;
                    vector<string> order_stats_row;
                    vector<string> CashFlow_row;
                    string date = date_formatting_basic(data[i][0]);
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
        {
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
                        CashFlow.push_back({date_formatting_basic(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n - j;
                    break;
                }
            }
            if (check == i)
            {
                if (curr_pos == -x)
                {

                    for (int k = 0; k < n; k++)
                    {
                        if (i + k >= data_length)
                        {
                            break;
                        }
                        CashFlow.push_back({date_formatting_basic(data[i + k][0]), to_string(cumulative_cashflow)});
                    }
                    i += n;
                }
                else
                {
                    curr_pos--;
                    vector<string> order_stats_row;
                    vector<string> CashFlow_row;
                    string date = date_formatting_basic(data[i][0]);
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
