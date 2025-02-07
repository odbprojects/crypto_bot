#include <iostream>
#include <curl/curl.h>
#include "api.h"
#include "order_manager.h"
#include "config/config.h"

int main() {
    std::cout << "Starting program..." << std::endl;
    
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return 1;
    }
    std::cout << "CURL initialized successfully" << std::endl;

    // Initialize OrderManager
    OrderManager orderManager;
    BinanceAPI api;

    // Test public endpoint
    std::cout << "Testing public endpoint..." << std::endl;
    std::string symbol = Config::getInstance().getSetting("default_market", "BTCUSDT");
    double currentPrice = orderManager.getCurrentPrice(symbol);
    if (currentPrice > 0) {
        std::cout << "Current " << symbol << " price: " << currentPrice << std::endl;
    } else {
        std::cerr << "Failed to get current price" << std::endl;
    }

    // Test account info
    std::cout << "\nFetching account information..." << std::endl;
    std::string accountInfo = orderManager.getAccountInfo();
    if (!accountInfo.empty()) {
        std::cout << "Account Info: " << accountInfo << std::endl;
    } else {
        std::cerr << "Failed to get account information" << std::endl;
    }

    // Example of placing orders (commented out for safety)
    /*
    std::cout << "\nPlacing test orders..." << std::endl;
    
    // Market order example
    std::string marketOrderResult = orderManager.placeMarketOrder(symbol, "BUY", 0.001);
    std::cout << "Market order result: " << marketOrderResult << std::endl;

    // Limit order example
    double limitPrice = currentPrice * 0.95; // 5% below current price
    std::string limitOrderResult = orderManager.placeLimitOrder(symbol, "BUY", 0.001, limitPrice);
    std::cout << "Limit order result: " << limitOrderResult << std::endl;
    */

    std::cout << "\nCleaning up..." << std::endl;
    curl_global_cleanup();
    Config::cleanup();
    std::cout << "Program finished." << std::endl;
    return 0;
}