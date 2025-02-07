#pragma once
#include <string>
#include "api.h"
#include "config/config.h"

class OrderManager {
private:
    BinanceAPI api;
    const Config& config;
    std::string api_key;
    std::string api_secret;
    std::string base_url;
    
    // Helper methods
    bool validateOrder(const std::string& symbol, 
                      const std::string& side, 
                      const std::string& type, 
                      double quantity, 
                      double price) const;

public:
    // Declare constructor (but don't define it here)
    OrderManager();
    
    // Place orders
    std::string place_order(const std::string& symbol, 
                           const std::string& side, 
                           const std::string& type, 
                           double quantity, 
                           double price = 0.0);
                           
    std::string placeMarketOrder(const std::string& symbol, 
                                const std::string& side, 
                                double quantity);
    
    std::string placeLimitOrder(const std::string& symbol, 
                               const std::string& side, 
                               double quantity, 
                               double price);
    
    // Get account information
    std::string getAccountInfo();
    
    // Get current price for a symbol
    double getCurrentPrice(const std::string& symbol);
};
