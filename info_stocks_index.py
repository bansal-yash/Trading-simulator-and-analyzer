import yfinance as yf

stock_symbol = "SBIN.NS"


def market_info(stock_symbol):
    stock_data = yf.Ticker(stock_symbol)
    data = stock_data.history(period="2d")
    info = yf.Ticker(stock_symbol).info

    closing_price = data["Close"].iloc[-1]
    open_price = data["Open"].iloc[-1]
    previous_close_price = data["Close"].iloc[-2]
    high_value = stock_data.history(period="1d")["High"].max()
    low_value = stock_data.history(period="1d")["Low"].min()
    volume_data = stock_data.history(period="1d")["Volume"].mean()
    avg_volume = info.get("averageVolume", None)
    market_cap = info.get("marketCap", None)
    total_debt = info.get("totalDebt", None)
    total_revenue = info.get("totalRevenue", None)
    pe_ratio = info.get("trailingPE")

    arr = [
        previous_close_price,
        open_price,
        closing_price,
        high_value,
        low_value,
        volume_data,
        avg_volume,
        pe_ratio,
        market_cap,
        total_debt,
        total_revenue,
    ]
    return arr


def market_info_index(index_symbol):
    nifty_ticker = yf.Ticker(index_symbol)
    nifty_info = nifty_ticker.info
    prev_close = nifty_info["previousClose"]
    open = nifty_info["open"]
    close = 0
    high = nifty_info["dayHigh"]
    low = nifty_info["dayLow"]
    vol = nifty_info["averageVolume"]
    avg_vol = nifty_info["averageVolume10days"]
    high_52 = nifty_info["fiftyTwoWeekHigh"]
    low_52 = nifty_info["fiftyTwoWeekLow"]
    avg_50 = nifty_info["fiftyDayAverage"]
    avg_200 = nifty_info["twoHundredDayAverage"]
    arr = [
        prev_close,
        open,
        close,
        high,
        low,
        vol,
        avg_vol,
        high_52,
        low_52,
        avg_50,
        avg_200,
    ]
    return arr
