#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include "api.h"
#include "backtester.h"
#include "order_manager.h"
#include "enhanced_strategy.h"

int main() {
    BinanceAPI api;
    OrderManager orderManager;
    
    // Create enhanced strategy with parameters
    EnhancedTradingStrategy strategy(api, orderManager, "BTCUSDT",
                                   12, 26, 9,   // MACD parameters
                                   14, 70, 30); // RSI parameters
    
    // Initialize backtester with strategy
    Backtester backtester(strategy);
    
    // Load and run backtest
    backtester.loadHistoricalData("tests/historical_data/BTCUSDT_1m_historical_data.csv");
    backtester.run();
    backtester.generateReport();
    
    return 0;
}