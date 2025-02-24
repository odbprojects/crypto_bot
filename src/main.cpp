#include <iostream>
#include <curl/curl.h>
#include "api.h"
#include "order_manager.h"
#include "SMA_strategy.h"
#include "config/config.h"
#include <nlohmann/json.hpp>
#include <chrono>
#include <thread>

int main() {
    std::cout << "Starting trading bot..." << std::endl;
    
    // Initialize CURL
    if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
        std::cerr << "Failed to initialize CURL" << std::endl;
        return 1;
    }
    std::cout << "CURL initialized successfully." << std::endl;

    // Initialize API and OrderManager
    BinanceAPI api;
    OrderManager orderManager;

    // Check if API is initialized correctly
    if (!api.is_initialized()) {
        std::cerr << "API initialization failed. Check config settings." << std::endl;
        return 1;
    }

    // Fetch current price for validation
    std::string symbol = Config::getInstance().getSetting("default_market", "BTCUSDT");
    double currentPrice = orderManager.getCurrentPrice(symbol);
    
    if (currentPrice > 0) {
        std::cout << "Current " << symbol << " price: " << currentPrice << std::endl;
    } else {
        std::cerr << "Failed to retrieve price data." << std::endl;
    }

    // Fetch account information
    std::cout << "\nFetching account information..." << std::endl;
    std::string accountInfo = orderManager.getAccountInfo();
    
    if (!accountInfo.empty()) {
        try {
            nlohmann::json j = nlohmann::json::parse(accountInfo);
            std::cout << "Account Info Summary:\n";
            std::cout << "- Can Trade: " << (j["canTrade"].get<bool>() ? "Yes" : "No") << std::endl;
            std::cout << "- Account Type: " << j["accountType"].get<std::string>() << std::endl;
            std::cout << "- Balances:\n";
            
            for (const auto& balance : j["balances"]) {
                double free = std::stod(balance["free"].get<std::string>());
                if (free > 0) {  // Only show non-zero balances
                    std::cout << "  " << balance["asset"].get<std::string>() << ": " 
                              << free << " (free) / "
                              << balance["locked"].get<std::string>() << " (locked)" << std::endl;
                }
            }
        } catch (const std::exception& e) {
            std::cerr << "Error parsing account info: " << e.what() << std::endl;
            std::cout << "Raw response: " << accountInfo << std::endl;
        }
    } else {
        std::cerr << "Failed to get account information." << std::endl;
    }

    // Initialize and run the trading strategy
    std::cout << "\nInitializing trading strategy..." << std::endl;
    SMAStrategy strategy(api, orderManager, symbol, 10, 50);

    std::cout << "Running strategy loop...\n" << std::endl;
    std::thread strategyThread(&SMAStrategy::run, &strategy);
    
    // Run strategy in a separate thread and allow manual exit
    while (true) {
        std::this_thread::sleep_for(std::chrono::seconds(10));
        std::cout << "Main thread is alive. Strategy running in background..." << std::endl;
    }

    // Cleanup before exiting
    curl_global_cleanup();
    Config::cleanup();
    
    std::cout << "Program finished." << std::endl;
    return 0;
}
