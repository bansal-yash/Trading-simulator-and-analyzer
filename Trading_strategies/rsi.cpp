#include <string>
#include <vector>

using namespace std;

string date_formatting_RSI(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

double avg_gain_loss(double arr[], int size, bool pos_or_neg)
{
    double sum = 0.0;
    if (pos_or_neg)
    {
        for (int i = 0; i < size; ++i)
        {
            if (arr[i] > 0)
            {
                sum += arr[i];
            }
        }
        return sum / size;
    }
    else
    {
        for (int i = 0; i < size; ++i)
        {
            if (arr[i] < 0)
            {
                sum -= arr[i];
            }
        }
        return sum / size;
    }
}

double RSI_strategy(const vector<vector<string>> &data, vector<vector<string>> &CashFlow, vector<vector<string>> &order_stats, int n, int x, double sell_thresh, double bought_thresh)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "Cashflow"});
    int curr_pos = 0;
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;
    double arr[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = stod(data[i + 1][1]) - stod(data[i][1]);
    }
    double avg_gain;
    double avg_loss;
    double RSI;
    for (int i = n; i < data_length; i++)
    {

        double price_today = stod(data[i][1]);
        arr[(i - 1) % n] = price_today - stod(data[i - 1][1]);
        string date = date_formatting_RSI(data[i][0]);
        avg_gain = avg_gain_loss(arr, n, true);
        avg_loss = avg_gain_loss(arr, n, false);
        double RS = avg_gain / avg_loss;
        RSI = stod("100") - (stod("100") / (1 + RS));

        vector<string> CashFlow_row;

        if (RSI < sell_thresh)
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
            CashFlow_row.push_back(date);
            CashFlow_row.push_back(to_string(cumulative_cashflow));
            CashFlow.push_back(CashFlow_row);
        }
        else if (RSI > bought_thresh)
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

    cumulative_cashflow += curr_pos * stod(data[data_length - 1][1]);
    return cumulative_cashflow;
}