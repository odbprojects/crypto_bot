#pragma once
#include <vector>
#include <string>
#include "trade.h"

class Backtester {
public:
    void logTrade(const Trade& trade);
    void announceTrade(const Trade& trade);
    void saveTradesToCSV(const std::string& filename);
    void analyzePerformance();

private:
    std::vector<Trade> trades;
    double totalPnL = 0.0;
    int winningTrades = 0;
    int losingTrades = 0;
};
