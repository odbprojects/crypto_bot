#include "backtester.h"
#include <iostream>
#include <fstream>
#include <iomanip>

void Backtester::logTrade(const Trade& trade) {
    trades.push_back(trade);
    totalPnL += trade.pnl;
    if (trade.pnl > 0) winningTrades++;
    else if (trade.pnl < 0) losingTrades++;
}

void Backtester::announceTrade(const Trade& trade) {
    std::cout << "Trade: " << trade.side << " " << trade.quantity << " " 
              << trade.symbol << " @ " << trade.price 
              << " PnL: " << trade.pnl << std::endl;
}

void Backtester::saveTradesToCSV(const std::string& filename) {
    std::ofstream file(filename);
    file << "Timestamp,Symbol,Side,Price,Quantity,PnL\n";
    
    for (const auto& trade : trades) {
        file << trade.timestamp << ","
             << trade.symbol << ","
             << trade.side << ","
             << trade.price << ","
             << trade.quantity << ","
             << trade.pnl << "\n";
    }
}

void Backtester::analyzePerformance() {
    int totalTrades = trades.size();
    double winRate = totalTrades > 0 ? (double)winningTrades / totalTrades * 100 : 0;
    
    std::cout << "\nPerformance Analysis:" << std::endl;
    std::cout << "Total Trades: " << totalTrades << std::endl;
    std::cout << "Winning Trades: " << winningTrades << std::endl;
    std::cout << "Losing Trades: " << losingTrades << std::endl;
    std::cout << "Win Rate: " << std::fixed << std::setprecision(2) << winRate << "%" << std::endl;
    std::cout << "Total PnL: " << totalPnL << std::endl;
}
