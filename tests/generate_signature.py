import hashlib
import hmac
import time
import requests

# Binance Testnet API Credentials
API_KEY = "wEc0P2U5SXeqK92yhlA1B3SRcyTqhWf8LMfhHITG6AYddHPYu6AQKBq7MhNAvFoA"
SECRET_KEY = "yaq6Ao7Y6eHU8cAtlPEau5X9kGahm8BvMpxfVEziE39b7nbVOZY0Z4PwkhqnIOb2"

# Endpoint and parameters
base_url = "https://testnet.binance.vision"
endpoint = "/api/v3/order"

# Create timestamp (Binance uses milliseconds)
timestamp = int(time.time() * 1000)

# Parameters for a Market Order
params = {
    "symbol": "BTCUSDT",
    "side": "BUY",
    "type": "MARKET",
    "quantity": 0.001,
    "timestamp": timestamp
}

# Convert parameters to query string
query_string = "&".join([f"{key}={value}" for key, value in params.items()])

# Generate HMAC-SHA256 Signature
signature = hmac.new(
    SECRET_KEY.encode('utf-8'), 
    query_string.encode('utf-8'), 
    hashlib.sha256
).hexdigest()

# Add signature to parameters
params["signature"] = signature

# Send request
headers = {"X-MBX-APIKEY": API_KEY}
response = requests.post(f"{base_url}{endpoint}", headers=headers, params=params)

# Print response
print(response.json())
