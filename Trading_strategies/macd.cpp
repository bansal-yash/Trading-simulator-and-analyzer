#include <string>
#include <vector>
#include <cmath>

using namespace std;

string date_formatting_macd(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

double calc_EWM_macd(double val_current, double EWM_prev, double alpha)
{
    return alpha * val_current + (1 - alpha) * EWM_prev;
}

double calc_MACD_macd(double short_EWM, double long_EWM)
{
    return short_EWM - long_EWM;
}
double MACD_strategy(const vector<vector<string>> &data, vector<vector<string>> &CashFlow, vector<vector<string>> &order_stats, int x)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "Cashflow"});
    int data_length = data.size() - 1;
    int current_position = 0;
    double cumulative_cashflow = 0;
    double short_EWM = stod(data[0][1]);
    double long_EWM = stod(data[0][1]);
    double MACD = 0;
    double Signal = 0;
    double two = 2.0;
    double thirteen = 13.0;
    double twenty_seven = 27.0;

    CashFlow.push_back({date_formatting_macd(data[0][0]), to_string(cumulative_cashflow)});
    for (int i = 1; i < data_length; i++)
    { // starting from i = 1 because Signal = MCD for start day or day0
        double price_today = stod(data[i][1]);
        string date = date_formatting_macd(data[i][0]);
        short_EWM = calc_EWM_macd(price_today, short_EWM, two / thirteen);
        long_EWM = calc_EWM_macd(price_today, long_EWM, two / twenty_seven);
        MACD = short_EWM - long_EWM;
        Signal = calc_EWM_macd(MACD, Signal, 0.2);
        vector<string> CashFlow_row;

        if (MACD > Signal)
        {
            if (current_position < x)
            {
                current_position++;
                cumulative_cashflow -= price_today;
                vector<string> order_stats_row;
                order_stats_row.push_back(date);
                order_stats_row.push_back("BUY");
                order_stats_row.push_back("1");
                order_stats_row.push_back(data[i][1]);
                order_stats.push_back(order_stats_row);
            }
            CashFlow_row.push_back(date);
            CashFlow_row.push_back(to_string(cumulative_cashflow));
            CashFlow.push_back(CashFlow_row);
        }
        else if (MACD < Signal)
        {
            if (current_position > -x)
            {
                current_position--;
                cumulative_cashflow += price_today;
                vector<string> order_stats_row;
                order_stats_row.push_back(date);
                order_stats_row.push_back("SELL");
                order_stats_row.push_back("1");
                order_stats_row.push_back(data[i][1]);
                order_stats.push_back(order_stats_row);
            }
            CashFlow_row.push_back(date);
            CashFlow_row.push_back(to_string(cumulative_cashflow));
            CashFlow.push_back(CashFlow_row);
        }
        else
        {
            CashFlow_row.push_back(date);
            CashFlow_row.push_back(to_string(cumulative_cashflow));
            CashFlow.push_back(CashFlow_row);
        }
    }

    cumulative_cashflow += current_position * stod(data[data_length - 1][1]);
    return cumulative_cashflow;
}