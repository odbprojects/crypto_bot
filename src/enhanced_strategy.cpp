// enhanced_strategy.cpp
#include "enhanced_strategy.h"
#include <iostream>
#include <numeric>
#include <algorithm>
#include <cmath>

EnhancedTradingStrategy::EnhancedTradingStrategy(BinanceAPI& api, OrderManager& orderManager,
                                               const std::string& symbol,
                                               int fastEMA, int slowEMA, int signalEMA,
                                               int rsiPeriod, double rsiOverbought, double rsiOversold)
    : api(api)
    , orderManager(orderManager)
    , symbol(symbol)
    , running(false)
    , fastEMA(fastEMA)
    , slowEMA(slowEMA)
    , signalEMA(signalEMA)
    , rsiPeriod(rsiPeriod)
    , rsiOverbought(rsiOverbought)
    , rsiOversold(rsiOversold) {}

void EnhancedTradingStrategy::run() {
    running = true;
    while (running) {
        // Real-time implementation details would go here
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void EnhancedTradingStrategy::stop() {
    running = false;
}

void EnhancedTradingStrategy::updateMarketData(double price, double volume) {
    priceHistory.push_back(price);
    volumeHistory.push_back(volume);
    // Keep a reasonable buffer size to avoid excessive memory usage
    const size_t MAX_HISTORY = 500;
    if (priceHistory.size() > MAX_HISTORY) {
        priceHistory.erase(priceHistory.begin());
        volumeHistory.erase(volumeHistory.begin());
    }
}

double EnhancedTradingStrategy::calculateSMA(int period) const {
    if (priceHistory.size() < period) return 0.0;
    
    return std::accumulate(
        priceHistory.end() - period,
        priceHistory.end(),
        0.0
    ) / period;
}

std::vector<double> EnhancedTradingStrategy::calculateEMA(const std::vector<double>& data, int period) const {
    std::vector<double> ema(data.size());
    if (data.size() < period) return ema;
    
    // Calculate SMA for the first EMA value
    double sum = 0;
    for (int i = 0; i < period; i++) {
        sum += data[i];
    }
    ema[period-1] = sum / period;
    
    // Calculate multiplier
    double multiplier = 2.0 / (period + 1);
    
    // Calculate EMA for remaining values
    for (size_t i = period; i < data.size(); i++) {
        ema[i] = (data[i] - ema[i-1]) * multiplier + ema[i-1];
    }
    
    return ema;
}

double EnhancedTradingStrategy::calculateRSI(int period) const {
    if (priceHistory.size() <= period) return 50.0; // Neutral if not enough data
    
    std::vector<double> gains;
    std::vector<double> losses;
    
    // Calculate price changes
    for (size_t i = priceHistory.size() - period; i < priceHistory.size(); i++) {
        if (i == 0) continue;
        double change = priceHistory[i] - priceHistory[i-1];
        if (change > 0) {
            gains.push_back(change);
            losses.push_back(0);
        } else {
            gains.push_back(0);
            losses.push_back(std::abs(change));
        }
    }
    
    // Calculate average gain and loss
    double avgGain = std::accumulate(gains.begin(), gains.end(), 0.0) / period;
    double avgLoss = std::accumulate(losses.begin(), losses.end(), 0.0) / period;
    
    // Calculate RSI
    if (avgLoss == 0) return 100.0;
    double rs = avgGain / avgLoss;
    return 100.0 - (100.0 / (1.0 + rs));
}

std::pair<std::vector<double>, std::vector<double>> EnhancedTradingStrategy::calculateMACD() const {
    if (priceHistory.size() < slowEMA) {
        return std::make_pair(std::vector<double>(), std::vector<double>());
    }
    
    // Calculate fast and slow EMAs
    std::vector<double> fastEMAValues = calculateEMA(priceHistory, fastEMA);
    std::vector<double> slowEMAValues = calculateEMA(priceHistory, slowEMA);
    
    // Calculate MACD line (fast EMA - slow EMA)
    std::vector<double> macdLine(priceHistory.size());
    for (size_t i = 0; i < priceHistory.size(); i++) {
        if (i < slowEMA - 1) {
            macdLine[i] = 0;
        } else {
            macdLine[i] = fastEMAValues[i] - slowEMAValues[i];
        }
    }
    
    // Calculate signal line (EMA of MACD line)
    std::vector<double> signalLine = calculateEMA(macdLine, signalEMA);
    
    return std::make_pair(macdLine, signalLine);
}

double EnhancedTradingStrategy::calculateATR(int period) const {
    if (priceHistory.size() < period) return 0.0;
    
    // Simulate high/low from close prices for this example
    std::vector<double> trueRanges;
    double prevClose = priceHistory[priceHistory.size() - period - 1];
    
    for (size_t i = priceHistory.size() - period; i < priceHistory.size(); i++) {
        double high = priceHistory[i] * 1.002; // Simulated high
        double low = priceHistory[i] * 0.998;  // Simulated low
        double close = priceHistory[i];
        
        // True Range calculation
        double tr1 = high - low;
        double tr2 = std::abs(high - prevClose);
        double tr3 = std::abs(low - prevClose);
        double tr = std::max({tr1, tr2, tr3});
        
        trueRanges.push_back(tr);
        prevClose = close;
    }
    
    // Calculate ATR as average of true ranges
    return std::accumulate(trueRanges.begin(), trueRanges.end(), 0.0) / period;
}

bool EnhancedTradingStrategy::isVolumeIncreasing() const {
    if (volumeHistory.size() < 10) return false;
    
    double recentVolume = std::accumulate(
        volumeHistory.end() - 3,
        volumeHistory.end(),
        0.0
    ) / 3;
    
    double prevVolume = std::accumulate(
        volumeHistory.end() - 10,
        volumeHistory.end() - 3,
        0.0
    ) / 7;
    
    return recentVolume > prevVolume * 1.2; // 20% volume increase
}

bool EnhancedTradingStrategy::isPriceAboveEMA(int period) const {
    if (priceHistory.size() < period) return false;
    
    std::vector<double> ema = calculateEMA(priceHistory, period);
    return priceHistory.back() > ema.back();
}

EnhancedTradingStrategy::TrendDirection EnhancedTradingStrategy::detectTrend() const {
    if (priceHistory.size() < 50) return SIDEWAYS;
    
    // Check if price is above key moving averages
    bool aboveEMA20 = isPriceAboveEMA(20);
    bool aboveEMA50 = isPriceAboveEMA(50);
    
    // Check recent price movement
    double priceChange = (priceHistory.back() - priceHistory[priceHistory.size() - 20]) / 
                          priceHistory[priceHistory.size() - 20] * 100;
    
    if (aboveEMA20 && aboveEMA50 && priceChange > 2.0) {
        return UPTREND;
    } else if (!aboveEMA20 && !aboveEMA50 && priceChange < -2.0) {
        return DOWNTREND;
    } else {
        return SIDEWAYS;
    }
}

bool EnhancedTradingStrategy::isVolatilityHigh() const {
    double atr = calculateATR();
    double currentPrice = priceHistory.back();
    
    // If ATR is more than 2% of current price, consider volatility high
    return atr > (currentPrice * 0.02);
}

// Support and resistance detection
bool EnhancedTradingStrategy::isSupportLevel(double price) const {
    if (priceHistory.size() < 20) return false;
    
    int touchCount = 0;
    double threshold = price * 0.005; // 0.5% threshold
    
    for (size_t i = priceHistory.size() - 20; i < priceHistory.size(); i++) {
        if (std::abs(priceHistory[i] - price) < threshold) {
            touchCount++;
            if (touchCount >= 3) return true;
        }
    }
    
    return false;
}

bool EnhancedTradingStrategy::isResistanceLevel(double price) const {
    return isSupportLevel(price); // Similar logic for this example
}

double EnhancedTradingStrategy::getLastPrice() const {
    if (priceHistory.empty()) return 0.0;
    return priceHistory.back();
}

// Signal generation with combined indicators
bool EnhancedTradingStrategy::shouldEnterLong() const {
    if (priceHistory.size() < slowEMA + 20) return false;
    
    double currentPrice = priceHistory.back();
    
    // Calculate EMAs
    double ema8 = calculateEMA(priceHistory, 8).back();
    double ema21 = calculateEMA(priceHistory, 21).back();
    double ema50 = calculateEMA(priceHistory, 50).back();
    
    // Trend alignment
    bool strongTrend = ema8 > ema21 && ema21 > ema50;
    bool priceAboveEMAs = currentPrice > ema8 && currentPrice > ema21;
    
    // RSI for momentum
    double rsi = calculateRSI(14);
    bool rsiGood = rsi > 40 && rsi < 65; // More conservative RSI range
    
    // MACD for trend confirmation
    auto macdData = calculateMACD();
    std::vector<double>& macdLine = macdData.first;
    std::vector<double>& signalLine = macdData.second;
    
    bool macdPositive = false;
    if (macdLine.size() > 2) {
        macdPositive = macdLine[macdLine.size()-1] > 0 && 
                      macdLine[macdLine.size()-1] > signalLine[signalLine.size()-1];
    }
    
    // Volume confirmation
    bool volumeGood = false;
    if (volumeHistory.size() > 10) {
        double avgVolume = std::accumulate(volumeHistory.end() - 10, volumeHistory.end(), 0.0) / 10;
        volumeGood = volumeHistory.back() > avgVolume;
    }
    
    // Price action: Check for higher lows
    bool higherLows = false;
    if (priceHistory.size() > 4) {
        double min1 = *std::min_element(priceHistory.end() - 2, priceHistory.end());
        double min2 = *std::min_element(priceHistory.end() - 4, priceHistory.end() - 2);
        higherLows = min1 > min2;
    }
    
    // Need at least 3 out of 5 conditions for entry
    int conditions = 0;
    conditions += (strongTrend && priceAboveEMAs) ? 1 : 0;
    conditions += rsiGood ? 1 : 0;
    conditions += macdPositive ? 1 : 0;
    conditions += volumeGood ? 1 : 0;
    conditions += higherLows ? 1 : 0;
    
    return conditions >= 3;
}

bool EnhancedTradingStrategy::shouldExitLong() const {
    if (priceHistory.size() < slowEMA + 20) return false;
    
    double currentPrice = priceHistory.back();
    double entryPrice = getLastPrice();
    
    // Calculate ATR for dynamic exits
    double atr = calculateATR(14);
    double atrMultiplier = 2.0;
    
    // Dynamic stop loss and take profit based on ATR
    double stopLossDistance = atr * atrMultiplier;
    double takeProfitDistance = atr * atrMultiplier * 1.5; // 1.5x the stop loss
    
    bool stopLoss = (currentPrice - entryPrice) < -stopLossDistance;
    bool takeProfit = (currentPrice - entryPrice) > takeProfitDistance;
    
    // Trend reversal signals
    double ema8 = calculateEMA(priceHistory, 8).back();
    double ema21 = calculateEMA(priceHistory, 21).back();
    bool trendReversal = currentPrice < ema8 && ema8 < ema21;
    
    // RSI exit condition
    double rsi = calculateRSI(14);
    bool rsiExit = rsi > 70 || rsi < 40;
    
    // MACD reversal
    auto macdData = calculateMACD();
    std::vector<double>& macdLine = macdData.first;
    bool macdReversal = macdLine.size() > 2 && 
                       macdLine[macdLine.size()-1] < 0 &&
                       macdLine[macdLine.size()-1] < macdLine[macdLine.size()-2];
    
    // Exit if stop loss or take profit hit, or if multiple reversal signals
    return stopLoss || takeProfit || (trendReversal && (rsiExit || macdReversal));
}

bool EnhancedTradingStrategy::shouldEnterShort() const {
    if (priceHistory.size() < slowEMA + 10) return false;
    
    // RSI conditions
    double rsi = calculateRSI(rsiPeriod);
    bool rsiCondition = rsi < 70 && rsi > 40; // Coming out of overbought but not oversold
    
    // MACD conditions
    auto macdData = calculateMACD();
    std::vector<double>& macdLine = macdData.first;
    std::vector<double>& signalLine = macdData.second;
    
    bool macdCrossunder = false;
    if (macdLine.size() > 2 && signalLine.size() > 2) {
        // Check for MACD crossunder (MACD line crosses below signal line)
        macdCrossunder = macdLine[macdLine.size()-2] >= signalLine[signalLine.size()-2] &&
                          macdLine[macdLine.size()-1] < signalLine[signalLine.size()-1];
    }
    
    // Trend conditions
    TrendDirection trend = detectTrend();
    bool trendCondition = trend == DOWNTREND;
    
    // Volume confirmation
    bool volumeCondition = isVolumeIncreasing();
    
    // Volatility filter
    bool volatilityCondition = !isVolatilityHigh();
    
    // Combined signal (prioritize trend with confirmation from other indicators)
    bool strongSignal = trendCondition && (macdCrossunder || rsiCondition) && volumeCondition;
    bool moderateSignal = macdCrossunder && rsiCondition && volatilityCondition;
    
    return strongSignal || moderateSignal;
}

bool EnhancedTradingStrategy::shouldExitShort() const {
    if (priceHistory.size() < slowEMA + 10) return false;
    
    // RSI conditions
    double rsi = calculateRSI(rsiPeriod);
    bool rsiOversold = rsi < rsiOversold;
    
    // MACD conditions
    auto macdData = calculateMACD();
    std::vector<double>& macdLine = macdData.first;
    std::vector<double>& signalLine = macdData.second;
    
    bool macdCrossover = false;
    if (macdLine.size() > 2 && signalLine.size() > 2) {
        // Check for MACD crossover (MACD line crosses above signal line)
        macdCrossover = macdLine[macdLine.size()-2] <= signalLine[signalLine.size()-2] &&
                         macdLine[macdLine.size()-1] > signalLine[signalLine.size()-1];
    }
    
    // Trend conditions
    TrendDirection trend = detectTrend();
    bool trendReversal = trend == UPTREND;
    
    // Take profit logic
    double entryPrice = 0.0; // Would track this in real implementation
    double currentPrice = getLastPrice();
    double profitTarget = 0.03; // 3% profit target
    bool profitTargetReached = (entryPrice > 0) && 
                              ((entryPrice - currentPrice) / entryPrice > profitTarget);
    
    // Stop loss logic
    double stopLossPercent = -0.01; // 1% stop loss
    bool stopLossTriggered = (entryPrice > 0) && 
                            ((entryPrice - currentPrice) / entryPrice < stopLossPercent);
    
    // Combined exit signals
    return rsiOversold || macdCrossover || trendReversal || 
           profitTargetReached || stopLossTriggered;
}