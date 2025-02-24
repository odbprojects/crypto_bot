// enhanced_strategy.h
#pragma once
#include <vector>
#include <string>
#include <deque>
#include "api.h"
#include "order_manager.h"

class EnhancedTradingStrategy {
public:
    EnhancedTradingStrategy(BinanceAPI& api, OrderManager& orderManager,
                          const std::string& symbol, 
                          int fastEMA = 12, 
                          int slowEMA = 26, 
                          int signalEMA = 9,
                          int rsiPeriod = 14, 
                          double rsiOverbought = 70, 
                          double rsiOversold = 30);

    void run();
    void stop();
    void updateMarketData(double price, double volume = 0);
    
    // Signal generators
    bool shouldEnterLong() const;
    bool shouldExitLong() const;
    bool shouldEnterShort() const;
    bool shouldExitShort() const;

    // For backtesting
    std::vector<double> getPriceHistory() const { return priceHistory; }
    std::vector<double> getVolumeHistory() const { return volumeHistory; }

private:
    BinanceAPI& api;
    OrderManager& orderManager;
    std::string symbol;
    bool running;
    
    // Price data
    std::vector<double> priceHistory;
    std::vector<double> volumeHistory;
    
    // Strategy parameters
    int fastEMA;
    int slowEMA;
    int signalEMA;
    int rsiPeriod;
    double rsiOverbought;
    double rsiOversold;
    
    // Technical indicators
    std::vector<double> calculateEMA(const std::vector<double>& data, int period) const;
    double calculateRSI(int period) const;
    std::pair<std::vector<double>, std::vector<double>> calculateMACD() const;
    double calculateATR(int period = 14) const;
    bool isPriceAboveEMA(int period) const;
    
    // Trend detection
    enum TrendDirection { UPTREND, DOWNTREND, SIDEWAYS };
    TrendDirection detectTrend() const;
    
    // Volume analysis
    bool isVolumeIncreasing() const;
    
    // Market conditions
    bool isVolatilityHigh() const;
    bool isSupportLevel(double price) const;
    bool isResistanceLevel(double price) const;
    
    // Helper functions
    double calculateSMA(int period) const;
    double getLastPrice() const;
};