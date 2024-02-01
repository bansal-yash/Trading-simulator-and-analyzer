import yfinance as yf
import plotly.graph_objects as go

def create_plot(ticker_symbol, period):
    interval = get_interval(period)
    data = yf.download(ticker_symbol, period=period, interval=interval)
    fig = go.Figure(data=go.Scatter(x=data.index, y=data['Close'], mode='lines'))
    fig.update_layout(
        title=f'Real-Time Line Chart - {ticker_symbol} ({period})',
        xaxis_title='Time',
        yaxis_title='Stock Price',
        template='plotly'
    )
    return fig

def update_plot(fig, ticker_symbol, period):
    interval = get_interval(period)
    data = yf.download(ticker_symbol, period=period, interval=interval)
    fig.update_traces(x=data.index, y=data['Close'])

def save_plotly_figure(fig, filename='figure.html'):
    fig.write_html(filename)

def get_interval(period):
    if period in ['1d', '1wk']:
        return '1m'  # 1 minute interval for 1d and 1wk periods
    elif period == '1y':
        return '30m'  # 30 minutes interval for 1yr period
    elif period in ['5y', 'max']:
        return '1d'  # 1 day interval for 5yr and max periods
    else:
        raise ValueError(f"Unsupported period: {period}")

# Example usage:
symbol = 'AAPL'
periods = ['1d', '1wk', '1y', '5y', 'max']

for period in periods:
    fig = create_plot(symbol, period=period)
    save_plotly_figure(fig, filename=f'{symbol}_{period}_chart.html')
