#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

string date_formatting_dma(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

double calculateMean(const double arr[], int size)
{
    double sum = 0;
    for (int i = 0; i < size; ++i)
    {
        sum += arr[i];
    }
    return sum / size;
}

double calculateStandardDeviation(const double arr[], int size, const double mean)
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

double computeChangeInStandardDeviation(double oldMean, double oldStdDeviation, double newValue, double oldValue, int size)
{
    double newMean = oldMean + (newValue - oldValue) / size;
    double newStdDeviation = std::sqrt((oldStdDeviation * oldStdDeviation * size - (oldValue - oldMean) * (oldValue - oldMean) + (newValue - newMean) * (newValue - newMean)) / size);
    return newStdDeviation - oldStdDeviation;
}

void DMA_strategy(const vector<vector<string>> &data, int n, int x, int p, vector<vector<string>> &order_stats, vector<vector<string>> &CashFlow)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "CashFlow"});
    int curr_pos = 0;
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;
    double arr[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = stod(data[i][1]);
    }
    for (int i = n; i < data_length; i++)
    {
        double mean = calculateMean(arr, n);
        double std_dev = calculateStandardDeviation(arr, n, mean);
        double ref_val_buy = mean + (p * std_dev);
        double ref_val_sell = mean - (p * std_dev);
        double today_price = stod(data[i][1]);
        string date = date_formatting_dma(data[i][0]);
        vector<string> CashFlow_row;

        if (today_price > ref_val_buy)
        {
            if (curr_pos < x)
            {
                vector<string> order_stats_row;
                cumulative_cashflow -= today_price;
                order_stats_row.push_back(date);
                order_stats_row.push_back("BUY");
                order_stats_row.push_back("1");
                order_stats_row.push_back(data[i][1]);
                CashFlow_row.push_back(date);
                CashFlow_row.push_back(to_string(cumulative_cashflow));
                CashFlow.push_back(CashFlow_row);
                order_stats.push_back(order_stats_row);
                curr_pos++;
            }
        }
        else if (today_price < ref_val_sell)
        {
            if (curr_pos > -x)
            {
                vector<string> order_stats_row;
                cumulative_cashflow += today_price;
                order_stats_row.push_back(date);
                order_stats_row.push_back("SELL");
                order_stats_row.push_back("1");
                order_stats_row.push_back(data[i][1]);
                CashFlow_row.push_back(date);
                CashFlow_row.push_back(to_string(cumulative_cashflow));
                CashFlow.push_back(CashFlow_row);
                order_stats.push_back(order_stats_row);
                curr_pos--;
            }
        }
        else
        {
            CashFlow_row.push_back(date);
            CashFlow_row.push_back(to_string(cumulative_cashflow));
            CashFlow.push_back(CashFlow_row);
        }
        arr[i % n] = today_price;
    }
}
