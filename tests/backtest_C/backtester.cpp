#include "backtester.h"
#include <fstream>
#include <sstream>
#include <iostream>
#include <cmath>
#include <algorithm>
#include <numeric>

void Backtester::loadHistoricalData(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    
    // Skip header
    std::getline(file, line);
    
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string token;
        HistoricalBar bar;
        
        std::getline(ss, bar.timestamp, ',');
        std::getline(ss, token, ','); bar.open = std::stod(token);
        std::getline(ss, token, ','); bar.high = std::stod(token);
        std::getline(ss, token, ','); bar.low = std::stod(token);
        std::getline(ss, token, ','); bar.close = std::stod(token);
        std::getline(ss, token, ','); bar.volume = std::stod(token);
        
        historicalData.push_back(bar);
    }
}

void Backtester::simulateTrade(const HistoricalBar& bar) {
    strategy->updateMarketData(bar.close, bar.volume);
    
    if (strategy->shouldEnterLong() && !inPosition) {
        // Risk only 1% of capital per trade
        double riskAmount = capital * 0.01;
        double atr = calculateATR(14, bar);
        double stopLoss = 2 * atr;  // 2 ATR stop loss
        
        // Calculate position size based on risk
        double quantity = riskAmount / stopLoss;
        quantity = std::floor(quantity * 1000) / 1000;  // Round to 3 decimals
        
        if (quantity * bar.close > capital * 0.1) {  // Max 10% of capital per trade
            quantity = (capital * 0.1) / bar.close;
        }
        
        // Record trade
        TradeResult trade;
        trade.entryPrice = bar.close;
        trade.type = "LONG";
        trade.entryTime = bar.timestamp;
        trade.quantity = quantity;
        
        capital -= quantity * bar.close * (1 + fees);
        currentPosition = quantity;
        inPosition = true;
        trades.push_back(trade);
    }
    else if (strategy->shouldExitLong() && inPosition) {
        trades.back().exitPrice = bar.close;
        trades.back().exitTime = bar.timestamp;
        
        double exitValue = currentPosition * bar.close * (1 - fees);
        capital += exitValue;
        
        trades.back().profit = exitValue - 
            (trades.back().quantity * trades.back().entryPrice * (1 + fees));
        
        currentPosition = 0;
        inPosition = false;
    }
}

void Backtester::run() {
    for (const auto& bar : historicalData) {
        simulateTrade(bar);
    }
}

void Backtester::generateReport() {
    // Calculate key metrics
    int totalTrades = trades.size();
    int profitableTrades = std::count_if(trades.begin(), trades.end(),
        [](const TradeResult& trade) { return trade.profit > 0; });
    
    double totalProfit = std::accumulate(trades.begin(), trades.end(), 0.0,
        [](double sum, const TradeResult& trade) { return sum + trade.profit; });
    
    double winRate = (double)profitableTrades / totalTrades * 100;
    double maxDrawdown = calculateDrawdown();
    double sharpeRatio = calculateSharpeRatio();
    
    // Print report
    std::cout << "\n=== Backtesting Results ===\n";
    std::cout << "Initial Capital: $" << initialCapital << "\n";
    std::cout << "Final Capital: $" << capital << "\n";
    std::cout << "Total Return: " << ((capital - initialCapital) / initialCapital * 100) << "%\n";
    std::cout << "Total Trades: " << totalTrades << "\n";
    std::cout << "Win Rate: " << winRate << "%\n";
    std::cout << "Max Drawdown: " << maxDrawdown << "%\n";
    std::cout << "Sharpe Ratio: " << sharpeRatio << "\n";
}

double Backtester::calculateDrawdown() {
    double maxCapital = initialCapital;
    double maxDrawdown = 0;
    
    for (const auto& trade : trades) {
        double currentCapital = capital + 
            (currentPosition * trade.exitPrice * (1 - fees));
        
        maxCapital = std::max(maxCapital, currentCapital);
        double drawdown = (maxCapital - currentCapital) / maxCapital * 100;
        maxDrawdown = std::max(maxDrawdown, drawdown);
    }
    
    return maxDrawdown;
}

double Backtester::calculateSharpeRatio() {
    std::vector<double> returns;
    double prevCapital = initialCapital;
    
    for (const auto& trade : trades) {
        double currentCapital = prevCapital + trade.profit;
        double returnPct = (currentCapital - prevCapital) / prevCapital;
        returns.push_back(returnPct);
        prevCapital = currentCapital;
    }
    
    double meanReturn = std::accumulate(returns.begin(), returns.end(), 0.0) / returns.size();
    
    double variance = 0;
    for (double r : returns) {
        variance += std::pow(r - meanReturn, 2);
    }
    variance /= returns.size();
    
    double stdDev = std::sqrt(variance);
    return meanReturn / stdDev * std::sqrt(252);  // Annualized Sharpe Ratio
}