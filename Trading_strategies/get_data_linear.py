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
start_date = start_date - relativedelta(days=n)

df = stock_df(symbol=sym, from_date=start_date, to_date=end_date, series="EQ")
df = df[["DATE", "CLOSE", "OPEN", "HIGH", "LOW", "VWAP", "NO OF TRADES"]]

file_name = sym + "linear.csv"
df.to_csv(file_name, index=False)
