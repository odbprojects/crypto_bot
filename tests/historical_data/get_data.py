import requests
import csv
import datetime
import time

# Binance API URL for historical Kline (candlestick) data
BINANCE_API_URL = "https://api.binance.us/api/v3/klines"

# Parameters
symbol = "BTCUSDT"  # Trading pair
interval = "1m"     # Timeframe: 1m, 5m, 15m, 1h, 1d, etc.
limit = 1000        # Number of candles per request (max 1000)
start_time = int(datetime.datetime(2024, 1, 1).timestamp() * 1000)  # Start from 2024-01-01

# Fetch historical data
params = {
    "symbol": symbol,
    "interval": interval,
    "limit": limit,
    "startTime": start_time,
}

# Add retry logic
max_retries = 3
for attempt in range(max_retries):
    try:
        response = requests.get(BINANCE_API_URL, params=params, timeout=10)
        response.raise_for_status()
        data = response.json()
        break
    except requests.exceptions.RequestException as e:
        if attempt == max_retries - 1:
            raise
        print(f"Attempt {attempt + 1} failed, retrying...")
        time.sleep(2)

# Save data to CSV
csv_filename = f"{symbol}_{interval}_historical_data.csv"
with open(csv_filename, "w", newline="") as f:
    writer = csv.writer(f)
    writer.writerow(["timestamp", "open", "high", "low", "close", "volume"])

    for row in data:
        timestamp = datetime.datetime.fromtimestamp(row[0] / 1000, tz=datetime.UTC).strftime('%Y-%m-%d %H:%M:%S')
        writer.writerow([timestamp, row[1], row[2], row[3], row[4], row[5]])

print(f"Data saved to {csv_filename}")
