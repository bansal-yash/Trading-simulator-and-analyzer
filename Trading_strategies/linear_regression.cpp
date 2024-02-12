#include <vector>
#include <string>

using namespace std;

void linearRegression(const vector<double> &Close, const vector<double> &Open, const vector<double> &VWAP, const vector<double> &Low, const vector<double> &High, const vector<double> &NoOfTrades, const vector<double> &ClosePrevDay, const vector<double> &OpenPrevDay, vector<double> &coefficients)
{
    int n = Close.size();

    vector<vector<double>> X(n, vector<double>(8, 1.0));
    for (int i = 0; i < n; ++i)
    {
        X[i][1] = ClosePrevDay[i];
        X[i][2] = OpenPrevDay[i];
        X[i][3] = VWAP[i];
        X[i][4] = Low[i];
        X[i][5] = High[i];
        X[i][6] = NoOfTrades[i];
        X[i][7] = Open[i];
    }

    vector<vector<double>> Xt(8, vector<double>(n));
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            Xt[i][j] = X[j][i];
        }
    }

    vector<vector<double>> XtX(8, vector<double>(8));
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            for (int k = 0; k < n; ++k)
            {
                XtX[i][j] += Xt[i][k] * X[k][j];
            }
        }
    }

    vector<double> Xty(8, 0.0);
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < n; ++j)
        {
            Xty[i] += Xt[i][j] * Close[j];
        }
    }

    vector<vector<double>> XtX_inv(8, vector<double>(8 * 2, 0.0));
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            XtX_inv[i][j] = XtX[i][j];
        }
        XtX_inv[i][i + 8] = 1.0;
    }
    for (int i = 0; i < 8; ++i)
    {
        int maxRow = i;
        for (int j = i + 1; j < 8; ++j)
        {
            if (abs(XtX_inv[j][i]) > abs(XtX_inv[maxRow][i]))
            {
                maxRow = j;
            }
        }
        swap(XtX_inv[i], XtX_inv[maxRow]);

        for (int j = i + 1; j < 8; ++j)
        {
            double factor = XtX_inv[j][i] / XtX_inv[i][i];
            for (int k = i; k < 16; ++k)
            {
                XtX_inv[j][k] -= XtX_inv[i][k] * factor;
            }
        }
    }
    for (int i = 7; i > 0; --i)
    {
        for (int j = i - 1; j >= 0; --j)
        {
            double factor = XtX_inv[j][i] / XtX_inv[i][i];
            for (int k = i; k < 16; ++k)
            {
                XtX_inv[j][k] -= XtX_inv[i][k] * factor;
            }
        }
    }
    for (int i = 0; i < 8; ++i)
    {
        double divisor = XtX_inv[i][i];
        for (int j = i; j < 16; ++j)
        {
            XtX_inv[i][j] /= divisor;
        }
    }

    vector<vector<double>> XtX_inverse(8, vector<double>(8));
    for (int i = 0; i < 8; ++i)
    {
        for (int j = 0; j < 8; ++j)
        {
            XtX_inverse[i][j] = XtX_inv[i][j + 8];
        }
    }

    coefficients.resize(8);
    for (int i = 0; i < 8; ++i)
    {
        coefficients[i] = 0.0;
        for (int j = 0; j < 8; ++j)
        {
            coefficients[i] += XtX_inverse[i][j] * Xty[j];
        }
    }
}

void train_data(const vector<vector<string>> &data, vector<double> &coefficients)
{
    vector<double> Close;
    vector<double> Open;
    vector<double> VWAP;
    vector<double> Low;
    vector<double> High;
    vector<double> NoOfTrades;
    vector<double> ClosePrevDay;
    vector<double> OpenPrevDay;

    int n = data.size();
    for (int i = 1; i < n; i++)
    {
        Close.push_back(stod(data[i][1]));
        Open.push_back(stod(data[i][2]));
        VWAP.push_back(stod(data[i - 1][5]));
        Low.push_back(stod(data[i - 1][4]));
        High.push_back(stod(data[i - 1][3]));
        NoOfTrades.push_back(stod(data[i - 1][6]));
        ClosePrevDay.push_back(stod(data[i - 1][1]));
        OpenPrevDay.push_back(stod(data[i - 1][2]));
    }
    linearRegression(Close, Open, VWAP, Low, High, NoOfTrades, ClosePrevDay, OpenPrevDay, coefficients);
}

string date_formatting_linear(string date)
{
    string s = "";
    s += date.substr(8, 2);
    s += "/";
    s += date.substr(5, 2);
    s += "/";
    s += date.substr(0, 4);
    return s;
}

void linear_regression_strategy(const vector<vector<string>> &data, const vector<double> coefficients, int x, int p, vector<vector<string>> &order_stats, vector<vector<string>> &CashFlow)
{
    order_stats.push_back({"Date", "Order_dir", "Quantity", "Price"});
    CashFlow.push_back({"Date", "CashFlow"});
    int curr_pos = 0;
    double cumulative_cashflow = 0;
    int data_length = data.size() - 1;

    for (int i = 1; i < data_length; i++)
    {
        double close = stod(data[i][1]);
        double open = stod(data[i][2]);
        double vwap = stod(data[i - 1][5]);
        double low = stod(data[i - 1][4]);
        double high = stod(data[i - 1][3]);
        double no_of_trades = stod(data[i - 1][6]);
        double close_prev = stod(data[i - 1][1]);
        double open_prev = stod(data[i - 1][2]);

        double predicted_price = coefficients[0] + (close_prev * coefficients[1]) + (open_prev * coefficients[2]) + (vwap * coefficients[3]) + (low * coefficients[4]) + (high * coefficients[5]) + (no_of_trades * coefficients[6]) + (open * coefficients[7]);
        double actual_price = close;
        double a = 100;
        double ref = p;

        string date = date_formatting_linear(data[i][0]);
        vector<string> CashFlow_row;

        if (predicted_price >= (1 + ref / a) * actual_price)
        {
            if (curr_pos < x)
            {
                vector<string> order_stats_row;
                cumulative_cashflow -= actual_price;
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
        else if (predicted_price <= (1 - ref / a) * actual_price)
        {
            if (curr_pos > -x)
            {
                vector<string> order_stats_row;
                cumulative_cashflow += actual_price;
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
    }
}
