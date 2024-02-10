from datetime import datetime
from dateutil.relativedelta import relativedelta
from jugaad_data.nse import stock_df
import sys

sym = sys.argv[1]
n = int(sys.argv[2])
start_date = sys.argv[3]
start_date = datetime.strptime(start_date, "%d/%m/%Y").date()
end_date = sys.argv[4]
end_date = datetime.strptime(end_date, "%d/%m/%Y").date()

df = stock_df(symbol=sym, from_date=start_date, to_date=end_date, series="EQ")
no_days = len(df)  # no of working days between start and end date
no_days_needed = no_days + n

new_start_date = start_date - relativedelta(days=2 * n)
min_start_date = start_date - relativedelta(months=1)
new_start_date = min(new_start_date, min_start_date)
df = stock_df(symbol=sym, from_date=new_start_date, to_date=end_date, series="EQ")

if len(df) < no_days_needed:
    new_start_date = start_date - relativedelta(days=3 * n)
    df = stock_df(symbol=sym, from_date=new_start_date, to_date=end_date, series="EQ")

df = df.head(no_days_needed)
df = df[["DATE", "CLOSE"]]

file_name = sym + ".csv"
df.to_csv(file_name, index=False)


# make strategy=BASIC symbol=SBIN n=5 x=2 start_date="b" end_date="a"
# make strategy=DMA symbol=SBIN n=50 x=3 p=2 start_date="a" end_date="b"
# make strategy="DMA++" symbol=SBIN x=4 p=5 n=14 max_hold_days=28 c1=2 c2=0.2 start_date="a" end_date="b"
# make strategy=MACD symbol=SBIN x=3 start_date="a" end_date="b"
# make strategy=RSI symbol=SBIN x=3 n=14 oversold_threshold=30 overbought_threshold=70 start_date="a" end_date="b"
# make strategy=ADX symbol=SBIN x=3 n=14 adx_threshold=25 start_date="a" end_date="b"
# make strategy="LINEAR_REGRESSION" symbol=SBIN x=3 p=2 train_start_date="a" train_end_date="b" start_date="c" end_date="d"
# make strategy="BEST_OF_ALL" symbol=SBIN start_date="a" end_date="b"
# make strategy=PAIRS symbol1=SBIN symbol2=ADANIENT x=5 n=20 threshold=2 start_date="a" end_date="b"
# make strategy=PAIRS symbol1=SBIN symbol2=ADANIENT x=5 n=20 threshold=2 stop_loss_threshold=4 start_date="a" end_date="b"
