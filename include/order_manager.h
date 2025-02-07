class OrderManager {
private:
    std::string api_key;
    std::string api_secret;
    std::string base_url = "https://testnet.binance.vision/api/v3/order";
    // ... other members ...

public:
    std::string place_order(const std::string &symbol, const std::string &side, 
                           const std::string &type, double quantity, double price = 0);
    // ... other methods ...
}; 