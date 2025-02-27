import time
import requests
import hmac
import hashlib

# Binance Testnet API Credentials
# Replace these with your own API keys from https://testnet.binance.vision/
API_KEY = "YOUR_API_KEY_HERE"
SECRET_KEY = "YOUR_SECRET_KEY_HERE"

# Example order parameters
symbol = "BTCUSDT"
side = "BUY"
type = "MARKET"
quantity = 0.001
timestamp = int(time.time() * 1000)

# Create the query string
query_string = f"symbol={symbol}&side={side}&type={type}&quantity={quantity}&timestamp={timestamp}"

# Generate HMAC SHA256 signature
signature = hmac.new(
    SECRET_KEY.encode('utf-8'),
    query_string.encode('utf-8'),
    hashlib.sha256
).hexdigest()

print(f"Query String: {query_string}")
print(f"Signature: {signature}") 