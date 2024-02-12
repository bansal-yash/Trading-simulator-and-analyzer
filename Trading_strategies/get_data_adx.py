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
no_days = len(df)
no_days_needed = no_days + n

new_start_date = start_date - relativedelta(days=2 * n)
min_start_date = start_date - relativedelta(months=1)
new_start_date = min(new_start_date, min_start_date)
df = stock_df(symbol=sym, from_date=new_start_date, to_date=end_date, series="EQ")

if len(df) < no_days_needed:
    new_start_date = start_date - relativedelta(days=3 * n)
    df = stock_df(symbol=sym, from_date=new_start_date, to_date=end_date, series="EQ")

df = df.head(no_days_needed)
df = df[["DATE", "CLOSE", "HIGH", "LOW", "PREV. CLOSE"]]

file_name = sym + ".csv"
df.to_csv(file_name, index=False)
