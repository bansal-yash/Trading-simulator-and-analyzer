#include <string>
#include <vector>
#include <cmath>

using namespace std;

string date_formatting_pairs(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

double calculateMean_pairs(const double arr[], int size)
{
    double sum = 0;
    for (int i = 0; i < size; ++i)
    {
        sum += arr[i];
    }
    return sum / size;
}

double calculateStandardDeviation_pairs(const double arr[], int size, const double mean)
{
    double squaredDifferencesSum = 0;
    for (int i = 0; i < size; ++i)
    {
        double difference = arr[i] - mean;
        squaredDifferencesSum += difference * difference;
    }

    double meanSquaredDifferences = squaredDifferencesSum / size;
    return std::sqrt(meanSquaredDifferences);
}

double Pairs_strategy(const vector<vector<string>> &data1, const vector<vector<string>> &data2, vector<vector<string>> &CashFlow, vector<vector<string>> &order_stats1, vector<vector<string>> &order_stats2, int x, int n, double thresh)
{
    order_stats1.push_back({"Date", "Order_dir", "Quantity", "Price"});
    order_stats2.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "Cashflow"});

    double spread_arr[n];
    for (int i = 0; i < n; i++)
    {
        spread_arr[i] = stod(data1[i][1]) - stod(data2[i][1]);
    }

    int curr_pos = 0;
    double cumulative_cashflow = 0.0;
    double rolling_mean;
    double rolling_std_dev;
    double z_score;
    double spread_today;
    int data_length = data1.size() - 1;
    for (int i = n; i < data_length; i++)
    {
        spread_today = stod(data1[i][1]) - stod(data2[i][1]);
        spread_arr[i % n] = spread_today;
        rolling_mean = calculateMean_pairs(spread_arr, n);
        rolling_std_dev = calculateStandardDeviation_pairs(spread_arr, n, rolling_mean);
        z_score = (spread_today - rolling_mean) / rolling_std_dev;
        string date = date_formatting_pairs(data1[i][0]);
        vector<string> CashFlow_row;
        if (z_score < -thresh)
        {

            if (curr_pos < x)
            {
                curr_pos++;
                cumulative_cashflow -= spread_today;
                vector<string> order_stats_row1;
                vector<string> order_stats_row2;

                order_stats_row1.push_back(date);
                order_stats_row1.push_back("BUY");
                order_stats_row1.push_back("1");
                order_stats_row1.push_back(data1[i][1]);

                order_stats_row2.push_back(date);
                order_stats_row2.push_back("SELL");
                order_stats_row2.push_back("1");
                order_stats_row2.push_back(data2[i][1]);

                order_stats1.push_back(order_stats_row1);
                order_stats2.push_back(order_stats_row2);
            }
        }
        else if (z_score > thresh)
        {

            if (curr_pos > -x)
            {
                curr_pos--;
                cumulative_cashflow += spread_today;
                vector<string> order_stats_row1;
                vector<string> order_stats_row2;

                order_stats_row1.push_back(date);
                order_stats_row1.push_back("SELL");
                order_stats_row1.push_back("1");
                order_stats_row1.push_back(data1[i][1]);

                order_stats_row2.push_back(date);
                order_stats_row2.push_back("BUY");
                order_stats_row2.push_back("1");
                order_stats_row2.push_back(data2[i][1]);

                order_stats1.push_back(order_stats_row1);
                order_stats2.push_back(order_stats_row2);
            }
        }

        CashFlow_row.push_back(date);
        CashFlow_row.push_back(to_string(cumulative_cashflow));
        CashFlow.push_back(CashFlow_row);
    }

    cumulative_cashflow += curr_pos * spread_today;
    return cumulative_cashflow;
}
