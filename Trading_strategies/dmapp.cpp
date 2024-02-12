#include <string>
#include <vector>
#include <cmath>
#include <iostream>

using namespace std;

// void print_arr(double arr[], int n)
// {
//     for (int i = 0; i < n; i++)
//     {
//         cout << arr[i] << " ";
//     }
//     cout << endl;
// }

string date_formatting_dmapp(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

double arraySum_dmapp(const double arr[], int size)
{
    double sum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        sum += arr[i];
    }
    return sum;
}

double arrayAbsSum_dmapp(const double arr[], int size)
{
    double sum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        sum += std::abs(arr[i]);
    }
    return sum;
}

double calc_ER_dmapp(const double arr[], int size)
{
    double sum = 0.0;
    double abs_sum = 0.0;
    for (int i = 0; i < size; ++i)
    {
        sum += arr[i];
        abs_sum += abs(arr[i]);
    }
    double ER = sum / abs_sum;
    return ER;
}

double calc_SF_dmapp(double SF_prev, double ER, double c1, double c2, int i)
{
    if (i == 0)
    {
        return 0.5;
    }
    double SF_today = SF_prev;
    double tmp1 = 2.0 * ER;
    tmp1 = tmp1 / (1.0 + c2);
    double tmp2 = (tmp1 - 1.0) / (tmp1 + 1.0);
    tmp2 -= SF_prev;
    tmp2 *= c1;
    SF_today += tmp2;
    return SF_today;
}

double calc_AMA_dmapp(double AMA_prev, double price_today, double SF_today, int i)
{
    if (i == 0)
    {
        return price_today;
    }
    double AMA_today = AMA_prev;
    double temp1 = price_today - AMA_prev;
    temp1 *= SF_today;
    AMA_today += temp1;
    return AMA_today;
}

double percent_difference_dmapp(double AMA, double price_today)
{
    return 100.0 * (AMA - price_today) / AMA;
}

double dmapp_strategy(const vector<vector<string>> &data, vector<vector<string>> &CashFlow, vector<vector<string>> &order_stats, int n, int x, double p, int max_hold_days, double c1, double c2)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    // CashFlow.push_back({"Date", "CashFlow", "AMA", "Price", "SF", "ER", "percent diff", ""});
    CashFlow.push_back({"Date", "CashFlow"});

    int curr_pos = 0;
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;
    bool check_buy_sell = false;

    double arr2[x];
    for (int i = 0; i < x; i++)
    {
        arr2[i] = -1;
    }                       // this contains the positions
    int first_buy_sell = 0; // stores the index of first share I bought/sell
    int this_buy_sell = 0;
    double arr[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = stod(data[i + 1][1]) - stod(data[i][1]);
    }

    double price_today = stod(data[n - 1][1]);
    double ER = 0;
    double SF_today = 0;
    double AMA_today = 0;

    for (int i = n; i < data_length; i++)
    {
        // cout << (i - 1) % n << ":::   ";
        price_today = stod(data[i][1]);
        arr[(i - 1) % n] = price_today - stod(data[i - 1][1]);
        // print_arr(arr, n);
        ER = calc_ER_dmapp(arr, n);
        SF_today = calc_SF_dmapp(SF_today, ER, c1, c2, i - n);
        AMA_today = calc_AMA_dmapp(AMA_today, price_today, SF_today, i - n);

        double percent_diff = percent_difference_dmapp(AMA_today, price_today);
        vector<string> CashFlow_row;
        string date = date_formatting_dmapp(data[i][0]);
        if (percent_diff > p)
        {
            if (curr_pos < x)
            {
                if (curr_pos < -1)
                {
                    // this condition is dealing with the first_buy_sell so no worry of max_hold_days
                    arr2[first_buy_sell] = -1;
                    first_buy_sell = (first_buy_sell + 1) % x;
                }
                else if (curr_pos == -1)
                {
                    // same as the last condition, no need to worry of max_hold_days
                    arr2[first_buy_sell] = -1;
                    first_buy_sell = 0;
                    this_buy_sell = 0;
                }
                else if (curr_pos == 0)
                {
                    arr2[first_buy_sell] = i;
                    arr2[this_buy_sell] = i;
                    this_buy_sell = (this_buy_sell + 1) % x;
                }
                else
                {
                    // we are buying the stock
                    // if max_hold_days says we neeed to sell the stock then in that case we do not write anything
                    // to order stats file but just update the date i.e remov the first buy and put it into today buy
                    if ((i - arr2[first_buy_sell]) >= max_hold_days)
                    {
                        // cout << "const from buy case  " + date << endl;
                        arr2[first_buy_sell] = -1;
                        first_buy_sell = (first_buy_sell + 1) % x;
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                        check_buy_sell = true; // it check if we have sell and buy on the same day or not
                                               // if we had done that then we will not update
                    }
                    else
                    {
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                    }
                }
                if (check_buy_sell == false)
                {
                    vector<string> order_stats_row;
                    cumulative_cashflow -= price_today;
                    order_stats_row.push_back(date);
                    order_stats_row.push_back("BUY");
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);
                    order_stats.push_back(order_stats_row);
                    curr_pos++;
                }
                check_buy_sell = false;
            }

            else if (curr_pos == x)
            {
                // need to check if we have to close any of the position
                // since we are not buyig any stock we can sell the stock if required
                if ((i - arr2[first_buy_sell]) >= max_hold_days)
                {
                    // cout << "const from buy case 2 " + date << endl;
                    arr2[first_buy_sell] == -1;
                    first_buy_sell = (first_buy_sell + 1) % x;
                    vector<string> order_stats_row;
                    cumulative_cashflow += price_today;
                    order_stats_row.push_back(date);
                    order_stats_row.push_back("SELL");
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);
                    order_stats.push_back(order_stats_row);
                    curr_pos--;
                }
            }
        }

        else if (percent_diff < -p)
        {
            if (curr_pos > -x)
            {
                if (curr_pos > 1)
                {
                    // this condition is dealing with the first_buy_sell so no worry of max_hold_days
                    arr2[first_buy_sell] = -1;
                    first_buy_sell = (first_buy_sell + 1) % x;
                }
                else if (curr_pos == 1)
                {
                    // same as the last condition, no need to worry of max_hold_days
                    arr2[first_buy_sell] = -1;
                    first_buy_sell = 0;
                    this_buy_sell = 0;
                }
                else if (curr_pos == 0)
                {
                    arr2[first_buy_sell] = i;
                    arr2[this_buy_sell] = i;
                    this_buy_sell = (this_buy_sell + 1) % x;
                }
                else
                {
                    // we are selling the stock
                    // if max_hold_days says we need to buy the stock then in that case we do not write anything
                    // to order stats file but just update the date i.e remove the first buy and put it into today buy
                    if ((i - arr2[first_buy_sell]) >= max_hold_days)
                    {
                        // cout << "const from sell case  " + date << endl;
                        arr2[first_buy_sell] = -1;
                        first_buy_sell = (first_buy_sell + 1) % x;
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                        check_buy_sell = true; // it check if we have buy and sell on the same day or not
                                               // if we had done that then we will not update order stats
                    }
                    else
                    {
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                    }
                }
                if (check_buy_sell == false)
                {
                    vector<string> order_stats_row;
                    cumulative_cashflow += price_today;
                    order_stats_row.push_back(date);
                    order_stats_row.push_back("SELL");
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);
                    order_stats.push_back(order_stats_row);
                    curr_pos--;
                }
                check_buy_sell = false;
            }

            else if (curr_pos == -x)
            {
                // need to check if we have to close any of the position
                // since we are not buyig any stock we can sell the stock if required
                if ((i - arr2[first_buy_sell]) >= max_hold_days)
                {
                    // cout << "const from sell case 2  " + date << endl;
                    arr2[first_buy_sell] == -1;
                    first_buy_sell = (first_buy_sell + 1) % x;
                    vector<string> order_stats_row;
                    cumulative_cashflow -= price_today;
                    order_stats_row.push_back(date);
                    order_stats_row.push_back("BUY");
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);
                    order_stats.push_back(order_stats_row);
                    curr_pos++;
                }
            }
        }

        else
        {
            if (curr_pos != 0)
            {
                if ((i - arr2[first_buy_sell]) >= max_hold_days)
                {
                    // cout << "const from null case  " + date << endl;
                    CashFlow_row.push_back(to_string(arr2[first_buy_sell]));
                    arr2[first_buy_sell] == -1;
                    first_buy_sell = (first_buy_sell + 1) % x; // modifying the first buy sell
                    vector<string> order_stats_row;
                    order_stats_row.push_back(date);
                    if (curr_pos < 0)
                    {
                        order_stats_row.push_back("BUY");
                        cumulative_cashflow -= price_today;
                        curr_pos++;
                    }
                    else
                    {
                        order_stats_row.push_back("SELL");
                        cumulative_cashflow += price_today;
                        curr_pos--;
                    }
                    order_stats_row.push_back("1");
                    order_stats_row.push_back(data[i][1]);

                    order_stats.push_back(order_stats_row);
                }
            }
        }
        CashFlow_row.push_back(date);
        CashFlow_row.push_back(to_string(cumulative_cashflow));
        CashFlow_row.push_back(to_string(AMA_today));
        CashFlow_row.push_back(to_string(price_today));
        CashFlow_row.push_back(to_string(SF_today));
        CashFlow_row.push_back(to_string(ER));
        CashFlow_row.push_back(to_string(percent_diff));
        CashFlow.push_back(CashFlow_row);
        // after the current day buy/sell is done then we will do update in the values to be used for next day

        // cout << date + " " << i << " " << curr_pos << endl;
    }

    cumulative_cashflow += curr_pos * stod(data[data_length - 1][1]);
    return cumulative_cashflow;
}