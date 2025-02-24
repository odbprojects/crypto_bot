#include "SMA_strategy.h"
#include <iostream>
#include <thread>
#include <chrono>
#include <numeric>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

// Update all TradingStrategy:: to SMAStrategy::
SMAStrategy::SMAStrategy(BinanceAPI& api, OrderManager& orderManager, 
                        const std::string& symbol, int shortPeriod, int longPeriod)
    : api(api)
    , orderManager(orderManager)
    , symbol(symbol)
    , shortPeriod(shortPeriod)
    , longPeriod(longPeriod)
    , running(false) {}

void SMAStrategy::run() {
    running = true;
    while (running) {
        std::cout << "Strategy running..." << std::endl;
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void SMAStrategy::stop() {
    running = false;
}

void SMAStrategy::updateMarketData(double historicalPrice) {
    priceHistory.push_back(historicalPrice);
    if (priceHistory.size() > longPeriod) {
        priceHistory.erase(priceHistory.begin());
    }
}

bool SMAStrategy::shouldEnterLong() const {
    if (priceHistory.size() < longPeriod) return false;
    
    double shortSMA = calculateSMA(shortPeriod);
    double longSMA = calculateSMA(longPeriod);
    
    return shortSMA > longSMA;
}

bool SMAStrategy::shouldExitLong() const {
    if (priceHistory.size() < longPeriod) return false;
    
    double shortSMA = calculateSMA(shortPeriod);
    double longSMA = calculateSMA(longPeriod);
    
    return shortSMA < longSMA;
}

double SMAStrategy::calculateSMA(int period) const {
    if (priceHistory.size() < period) return 0.0;
    
    double sum = std::accumulate(
        priceHistory.end() - period,
        priceHistory.end(),
        0.0
    );
    return sum / period;
} 