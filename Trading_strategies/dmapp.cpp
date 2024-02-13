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
    return 100.0 * (price_today - AMA) / AMA;
}

double dmapp_strategy(const vector<vector<string>> &data, vector<vector<string>> &CashFlow, vector<vector<string>> &order_stats, int n, int x, double p, int max_hold_days, double c1, double c2)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "CashFlow", "AMA", "Price", "SF", "ER", "percent diff", ""});
    // CashFlow.push_back({"Date", "CashFlow"});

    int curr_pos = 0;
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;
    bool check_buy_sell = false;

    double arr2[x];
    for (int i = 0; i < x; i++)
    {
        arr2[i] = -1;
    } // this contains the positions

    int first_buy_sell = 0; // stores the index of first share I bought/sell
    int this_buy_sell = 0;  // stores where next buy.sell should position at
    double arr[n];
    for (int i = 0; i < n; i++)
    {
        arr[i] = stod(data[i + 1][1]) - stod(data[i][1]);
    }

    double price_today = stod(data[n - 1][1]);
    double ER = 0;
    double SF_today = 0.5;
    double AMA_today = stod(data[n][1]);

    int curr_pos_at_start = 0;
    for (int i = n; i < data_length; i++)
    {
        curr_pos_at_start = curr_pos;
        price_today = stod(data[i][1]);

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
                    // arr2[this_buy_sell] = i; since first_buy_sell = this_buy_sell, this commented operation
                    // was redundant, actually first_buy_sell = 0, but I have used directly first_buy_sell for better clarity
                    this_buy_sell = (this_buy_sell + 1) % x;
                }
                else
                {
                    // we are buying the stock
                    // if max_hold_days says we need to sell the stock then in that case we do not write anything
                    // to order stats file but just update the date i.e remov the first buy and put it into today buy
                    if ((i - arr2[first_buy_sell]) >= max_hold_days)
                    {
                        arr2[first_buy_sell] = -1;
                        first_buy_sell = (first_buy_sell + 1) % x;
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                        curr_pos--;
                    }
                    else
                    {
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                    }
                }

                curr_pos++;
            }

            else if (curr_pos == x)
            {
                // need to check if we have to close any of the position
                // since we are not buying any stock we can sell the stock if required
                if ((i - arr2[first_buy_sell]) >= max_hold_days)
                {
                    arr2[first_buy_sell] == -1;
                    first_buy_sell = (first_buy_sell + 1) % x;
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
                    // same reasoning as the buying stock case
                    this_buy_sell = (this_buy_sell + 1) % x;
                }
                else
                {
                    // we are selling the stock
                    // if max_hold_days says we need to buy the stock then in that case we do not write anything
                    // to order stats file but just update the date(of first buy and this buy) i.e remove the first buy and put it into today buy
                    if ((i - arr2[first_buy_sell]) >= max_hold_days)
                    {
                        arr2[first_buy_sell] = -1;
                        first_buy_sell = (first_buy_sell + 1) % x;
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                        curr_pos++;
                    }
                    else
                    {
                        arr2[this_buy_sell] = i;
                        this_buy_sell = (this_buy_sell + 1) % x;
                    }
                }
                curr_pos--;
            }

            else if (curr_pos == -x)
            {
                // need to check if we have to close any of the position
                // since we are not selling any stock we can buy the stock if required
                if ((i - arr2[first_buy_sell]) >= max_hold_days)
                {
                    arr2[first_buy_sell] == -1;
                    first_buy_sell = (first_buy_sell + 1) % x;
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

                    if (curr_pos < 0)
                    {
                        curr_pos++;
                    }
                    else
                    {
                        curr_pos--;
                    }
                }
            }
        }

        int change = curr_pos - curr_pos_at_start;

        if (change != 0)
        {
            vector<string> order_stats_row;
            cumulative_cashflow -= change * price_today;
            order_stats_row.push_back(date);
            if (change < 0)
            {
                order_stats_row.push_back("BUY");
            }
            else
            {
                order_stats_row.push_back("BUY");
            }
            order_stats_row.push_back("1");
            order_stats_row.push_back(data[i][1]);
            order_stats.push_back(order_stats_row);
        }

        CashFlow_row.push_back(date);
        CashFlow_row.push_back(to_string(cumulative_cashflow));
        CashFlow_row.push_back(to_string(AMA_today));
        CashFlow_row.push_back(to_string(price_today));
        CashFlow_row.push_back(to_string(SF_today));
        CashFlow_row.push_back(to_string(ER));
        CashFlow_row.push_back(to_string(percent_diff));
        CashFlow.push_back(CashFlow_row);

        // cout << date + " " << first_buy_sell << "   " << date << "   " << arr2[first_buy_sell] << " i= " << to_string(i) <<endl;
        if (i < data_length - 1)
        {
            arr[i % n] = stod(data[i + 1][1]) - price_today;
            price_today = stod(data[i + 1][1]);
            ER = calc_ER_dmapp(arr, n);
            SF_today = calc_SF_dmapp(SF_today, ER, c1, c2, 1);
            AMA_today = calc_AMA_dmapp(AMA_today, price_today, SF_today, 1);
        }
    }

    cumulative_cashflow += curr_pos * stod(data[data_length - 1][1]);
    return cumulative_cashflow;
}
