#include "strategy.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <numeric>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Constructor
TradingStrategy::TradingStrategy(BinanceAPI& api, OrderManager& orderManager, 
                               const std::string& symbol, int shortPeriod, int longPeriod)
    : api(api)
    , orderManager(orderManager)
    , symbol(symbol)
    , shortPeriod(shortPeriod)
    , longPeriod(longPeriod)
    , running(false) {}

// Run strategy
void TradingStrategy::run() {
    running = true;
    while (running) {
        std::cout << "Strategy running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void TradingStrategy::stop() {
    running = false;
}

void TradingStrategy::updateMarketData(double historicalPrice) {
    priceHistory.push_back(historicalPrice);
    if (priceHistory.size() > longPeriod) {
        priceHistory.erase(priceHistory.begin());
    }
}
