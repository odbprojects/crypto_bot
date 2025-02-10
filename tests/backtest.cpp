#include <iostream>
#include <vector>
#include <chrono>
#include "backtester.h"
#include "order_manager.h"
#include "config/config.h"

int main() {
    std::cout << "Starting backtest...\n";
    
    OrderManager orderManager;
    Backtester backtester;

    std::string symbol = "BTCUSDT";
    double initialBalance = 10000.0; // Simulated balance
    double currentBalance = initialBalance;

    std::vector<double> historicalPrices = {/* Load historical data here */};

    for (size_t i = 1; i < historicalPrices.size(); i++) {
        double price = historicalPrices[i];
        double prevPrice = historicalPrices[i - 1];

        std::string timestamp = std::to_string(i); // Replace with real timestamp
        std::string side;
        double quantity = 0.01; // Example trade size
        double pnl = 0.0;

        if (price > prevPrice) {
            side = "BUY";
        } else {
            side = "SELL";
        }

        if (side == "BUY") {
            pnl = (historicalPrices[i] - historicalPrices[i - 1]) * quantity;
        } else {
            pnl = (historicalPrices[i - 1] - historicalPrices[i]) * quantity;
        }

        currentBalance += pnl;
        
        Trade trade(timestamp, symbol, side, price, quantity, pnl);
        backtester.logTrade(trade);
        backtester.announceTrade(trade);
    }

    std::cout << "\nBacktest complete. Saving results...\n";
    backtester.saveTradesToCSV("trade_history.csv");
    backtester.analyzePerformance();

    return 0;
}
