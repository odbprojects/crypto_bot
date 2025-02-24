#pragma once
#include <string>
#include <chrono>

class Trade {
public:
    Trade(const std::string& timestamp, const std::string& symbol, 
          const std::string& side, double price, double quantity, double pnl)
        : timestamp(timestamp)
        , symbol(symbol)
        , side(side)
        , price(price)
        , quantity(quantity)
        , pnl(pnl) {}

    std::string timestamp;
    std::string symbol;
    std::string side;
    double price;
    double quantity;
    double pnl;
};
