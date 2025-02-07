#include "order_manager.h"
#include <openssl/hmac.h>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <cstdlib> // For getenv()
#include <nlohmann/json.hpp>
#include <iostream>
#include <curl/curl.h>
#include "config/config.h"

using json = nlohmann::json;

// Helper function to safely get environment variables
std::string get_env_var(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    return val == nullptr ? "" : std::string(val);
}

// Function to generate HMAC SHA256 signature
std::string hmac_sha256(const std::string &data, const std::string &key) {
    unsigned char* digest = HMAC(EVP_sha256(), key.c_str(), key.length(), 
                                 (unsigned char*)data.c_str(), data.length(), nullptr, nullptr);
    
    std::ostringstream hex_stream;
    for (int i = 0; i < 32; i++)
        hex_stream << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];

    return hex_stream.str();
}

// Function to get current timestamp
long get_timestamp() {
    return std::time(nullptr) * 1000;
}

// Add these as member variables in the OrderManager class header file
// and remove these global variables
std::string BASE_URL = "https://testnet.binance.vision/api/v3/order";
std::string API_KEY;
std::string SECRET_KEY;

// Update the place_order function to be a member of OrderManager
std::string OrderManager::place_order(const std::string& symbol, 
                                    const std::string& side, 
                                    const std::string& type, 
                                    double quantity, 
                                    double price) {
    CURL *curl = curl_easy_init();
    if (!curl) {
        std::cerr << "Failed to initialize CURL." << std::endl;
        return "";
    }

    std::ostringstream params;
    params << "symbol=" << symbol << "&side=" << side 
           << "&type=" << type << "&quantity=" << quantity 
           << "&timestamp=" << std::time(nullptr) * 1000;

    if (type == "LIMIT") params << "&price=" << price << "&timeInForce=GTC";

    std::string query = params.str();
    std::string signature = hmac_sha256(query, api_secret);
    query += "&signature=" + signature;

    std::string url = base_url + "?" + query;
    
    struct curl_slist *headers = nullptr;
    headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + api_key).c_str());

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_POST, 1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);

    CURLcode res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK) ? "Order Placed" : "Order Failed";
}

OrderManager::OrderManager() 
    : config(Config::getInstance()) {
    api_key = config.getApiKey();
    api_secret = config.getApiSecret();
    base_url = config.getSetting("base_url") + "/api/v3/order";
}

bool OrderManager::validateOrder(const std::string& symbol, 
                               const std::string& side, 
                               const std::string& type, 
                               double quantity, 
                               double price) const {
    // Get minimum order size from config
    double min_order_size = std::stod(config.getSetting("min_order_size", "10.0"));
    
    if (quantity * price < min_order_size) {
        std::cerr << "Order size too small. Minimum is " << min_order_size << " USD" << std::endl;
        return false;
    }
    
    // Validate side
    if (side != "BUY" && side != "SELL") {
        std::cerr << "Invalid side. Must be BUY or SELL" << std::endl;
        return false;
    }
    
    // Validate type
    if (type != "MARKET" && type != "LIMIT") {
        std::cerr << "Invalid order type. Must be MARKET or LIMIT" << std::endl;
        return false;
    }
    
    return true;
}

std::string OrderManager::placeMarketOrder(const std::string& symbol, 
                                         const std::string& side, 
                                         double quantity) {
    if (!validateOrder(symbol, side, "MARKET", quantity, getCurrentPrice(symbol))) {
        return "Validation failed";
    }
    
    std::ostringstream query;
    query << "symbol=" << symbol 
          << "&side=" << side 
          << "&type=MARKET"
          << "&quantity=" << std::fixed << std::setprecision(8) << quantity;
    
    return api.send_signed_request("/api/v3/order", query.str());
}

std::string OrderManager::placeLimitOrder(const std::string& symbol, 
                                        const std::string& side, 
                                        double quantity, 
                                        double price) {
    if (!validateOrder(symbol, side, "LIMIT", quantity, price)) {
        return "Validation failed";
    }
    
    std::ostringstream query;
    query << "symbol=" << symbol 
          << "&side=" << side 
          << "&type=LIMIT"
          << "&timeInForce=GTC"
          << "&quantity=" << std::fixed << std::setprecision(8) << quantity
          << "&price=" << std::fixed << std::setprecision(2) << price;
    
    return api.send_signed_request("/api/v3/order", query.str());
}

std::string OrderManager::getAccountInfo() {
    return api.send_signed_request("/api/v3/account", "");
}

double OrderManager::getCurrentPrice(const std::string& symbol) {
    std::string response = api.send_public_request("/api/v3/ticker/price?symbol=" + symbol);
    
    try {
        json j = json::parse(response);
        return std::stod(j["price"].get<std::string>());
    } catch (const std::exception& e) {
        std::cerr << "Error parsing price: " << e.what() << std::endl;
        return 0.0;
    }
}
