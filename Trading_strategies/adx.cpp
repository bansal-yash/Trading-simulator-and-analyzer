#include <string>
#include <vector>
#include <cmath>
#include <iostream>
using namespace std;

string date_formatting_adx(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

double calc_EWM_adx(double val_current, double EWM_prev, double alpha, int i)
{
    if (i == 0)
    {
        return val_current;
    }
    return alpha * val_current + (1 - alpha) * EWM_prev;
}

double ADX_strategy(const vector<vector<string>> &data, vector<vector<string>> &CashFlow, vector<vector<string>> &order_stats, int x, int n, double adx_thres)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "CashFlow"});

    int curr_pos = 0;
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;
    double price_today;
    double today_high, prev_high, today_low, prev_low, prev_close;
    double TR;
    double true_rng;
    double DM_plus, DM_minus;
    double DI_plus, DI_minus;
    double ATR;
    double alpha = 2.0 / (1 + n);
    double DX;
    double ADX;
    prev_high = stod(data[0][2]);
    prev_low = stod(data[0][3]);
    for (int i = 1; i < data_length; i++)
    {
        today_high = stod(data[i][2]);
        today_low = stod(data[i][3]);
        prev_close = stod(data[i][4]);
        TR = max(today_high - today_low, max(today_high - prev_close, today_low - prev_close));
        price_today = stod(data[i][1]);
        DM_plus = max(0.0, today_high - prev_high);
        DM_minus = max(0.0, today_low - prev_low);
        ATR = calc_EWM_adx(TR, ATR, alpha, i - 1);
        DI_plus = calc_EWM_adx(DM_plus / ATR, DI_plus, alpha, i - 1);
        DI_minus = calc_EWM_adx(DM_minus / ATR, DI_plus, alpha, i - 1);
        DX = (DI_plus - DI_minus) / (DI_plus + DI_minus); // There should be abs in numerator
        DX = DX * 100;
        if (ATR == 0 or ((DI_plus + DI_minus) == 0))
        {
            DX = 0;
            prev_high = today_high;
            prev_low = today_low;
            vector<string> CashFlow_row;
            string date = date_formatting_adx(data[i][0]);
            CashFlow_row.push_back(date);
            CashFlow_row.push_back(to_string(cumulative_cashflow));
            CashFlow.push_back(CashFlow_row);
            continue;
        }

        ADX = calc_EWM_adx(DX, ADX, alpha, i - 1);

        string date = date_formatting_adx(data[i][0]);

        vector<string> CashFlow_row;

        if (ADX > adx_thres)
        {
            if (curr_pos < x)
            {
                curr_pos++;
                cumulative_cashflow -= price_today;
                vector<string> order_stats_row;
                order_stats_row.push_back(date);
                order_stats_row.push_back("BUY");
                order_stats_row.push_back("1");
                order_stats_row.push_back(data[i][1]);
                order_stats.push_back(order_stats_row);
            }
        }
        else if (ADX < adx_thres)
        {
            if (curr_pos > -x)
            {
                curr_pos--;
                cumulative_cashflow += price_today;
                vector<string> order_stats_row;
                order_stats_row.push_back(date);
                order_stats_row.push_back("SELL");
                order_stats_row.push_back("1");
                order_stats_row.push_back(data[i][1]);

                order_stats.push_back(order_stats_row);
            }
        }
        CashFlow_row.push_back(date);
        CashFlow_row.push_back(to_string(cumulative_cashflow));
        CashFlow.push_back(CashFlow_row);

        prev_high = today_high;
        prev_low = today_low;
    }

    cumulative_cashflow += curr_pos * stod(data[data_length - 1][1]);
    return cumulative_cashflow;
}