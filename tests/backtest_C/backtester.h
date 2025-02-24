// backtesting.h
#pragma once
#include <vector>
#include <string>
#include <map>
#include "enhanced_strategy.h"

struct TradeResult {
    double entryPrice;
    double exitPrice;
    std::string type;  // "LONG" or "SHORT"
    std::string entryTime;
    std::string exitTime;
    double profit;
    double quantity;
};

class Backtester {
public:
    // Changed constructor to use EnhancedTradingStrategy
    Backtester(EnhancedTradingStrategy& strategy, double initialCapital = 10000.0)
        : strategy(&strategy), capital(initialCapital), initialCapital(initialCapital) {}

    void loadHistoricalData(const std::string& filename);
    void run();
    void generateReport();

private:
    struct HistoricalBar {
        std::string timestamp;
        double open;
        double high;
        double low;
        double close;
        double volume;
    };

    // Add ATR calculation helper functions
    double calculateTR(const HistoricalBar& current, const HistoricalBar& prev) const {
        double hl = current.high - current.low;
        double hc = std::abs(current.high - prev.close);
        double lc = std::abs(current.low - prev.close);
        return std::max({hl, hc, lc});
    }
    
    double calculateATR(int period, const HistoricalBar& bar) const {
        if (historicalData.empty() || period <= 0) return 0.0;
        
        size_t currentIndex = 0;
        // Find current bar's index
        for (size_t i = 0; i < historicalData.size(); i++) {
            if (historicalData[i].timestamp == bar.timestamp) {
                currentIndex = i;
                break;
            }
        }
        
        if (currentIndex < period) return 0.0;
        
        // Calculate initial TR sum
        double trSum = 0.0;
        for (size_t i = currentIndex - period + 1; i <= currentIndex; i++) {
            trSum += calculateTR(historicalData[i], historicalData[i-1]);
        }
        
        return trSum / period;
    }

    EnhancedTradingStrategy* strategy;
    std::vector<HistoricalBar> historicalData;
    std::vector<TradeResult> trades;
    double capital;
    double initialCapital;
    
    // Portfolio tracking
    double currentPosition = 0.0;
    bool inPosition = false;
    double fees = 0.001;  // 0.1% trading fee

    void simulateTrade(const HistoricalBar& bar);
    double calculateDrawdown();
    double calculateSharpeRatio();
};