#pragma once

#include "api.h"
#include "order_manager.h"
#include <string>
#include <atomic>
#include <vector>

class TradingStrategy {
public:
    TradingStrategy(BinanceAPI& api, OrderManager& orderManager, 
                   const std::string& symbol, int shortPeriod, int longPeriod);
    
    void run();
    void stop();
    void updateMarketData(double historicalPrice);

private:
    BinanceAPI& api;
    OrderManager& orderManager;
    std::string symbol;
    int shortPeriod;
    int longPeriod;
    std::atomic<bool> running;
    double lastPrice;
    std::vector<double> priceHistory;
};
