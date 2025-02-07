#pragma once
#include <string>
#include <chrono>
#include <thread>
#include "config/config.h"

class BinanceAPI {
private:
    const Config& config;
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp);
    std::string hmac_sha256(const std::string &key, const std::string &data);

public:
    BinanceAPI() : config(Config::getInstance()) {}
    std::string send_signed_request(const std::string &endpoint, const std::string &query);
    std::string send_public_request(const std::string &endpoint);
    bool is_initialized() const { return config.isValid(); }
};