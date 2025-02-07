#include "config.h"
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;
Config* Config::instance = nullptr;

std::string Config::getEnvVar(const std::string& key) {
    const char* val = std::getenv(key.c_str());
    return val == nullptr ? "" : std::string(val);
}

void Config::loadEnvironmentVariables() {
    // First try to load from .env file
    std::ifstream env_file(".env");
    std::string line;
    std::map<std::string, std::string> env_vars;
    
    if (env_file.is_open()) {
        while (std::getline(env_file, line)) {
            // Skip empty lines and comments
            if (line.empty() || line[0] == '#') continue;
            
            // Find the equals sign
            size_t pos = line.find('=');
            if (pos != std::string::npos) {
                std::string key = line.substr(0, pos);
                std::string value = line.substr(pos + 1);
                env_vars[key] = value;
            }
        }
    }

    // Load API credentials from environment or .env file
    api_key = getEnvVar("TESTNET_API_KEY");
    if (api_key.empty() && env_vars.count("TESTNET_API_KEY")) {
        api_key = env_vars["TESTNET_API_KEY"];
    }
    
    // Try both secret key environment variable names
    api_secret = getEnvVar("TESTNET_SECRET_KEY"); // Try the name from .env first
    if (api_secret.empty()) {
        api_secret = getEnvVar("TESTNET_API_SECRET"); // Try alternate name
    }
    if (api_secret.empty() && env_vars.count("TESTNET_SECRET_KEY")) {
        api_secret = env_vars["TESTNET_SECRET_KEY"];
    }
    
    // Debug logging
    std::cout << "Loading environment variables..." << std::endl;
    std::cout << "API Key found: " << (!api_key.empty() ? "Yes" : "No") << std::endl;
    std::cout << "API Secret found: " << (!api_secret.empty() ? "Yes" : "No") << std::endl;
    if (!api_secret.empty()) {
        std::cout << "Secret length: " << api_secret.length() << std::endl;
    }
    
    // Load base URL
    std::string base_url = getEnvVar("TESTNET_BASE_URL");
    if (base_url.empty() && env_vars.count("TESTNET_BASE_URL")) {
        base_url = env_vars["TESTNET_BASE_URL"];
    }
    if (!base_url.empty()) {
        settings["base_url"] = base_url;
        std::cout << "Using base URL from env: " << base_url << std::endl;
    }
}

void Config::loadConfigFile() {
    try {
        std::ifstream config_file("config/config.json");
        if (config_file.is_open()) {
            json j;
            config_file >> j;
            
            // Only override API credentials if they're not set by environment variables
            if (api_key.empty() && j.contains("api_key")) {
                api_key = j["api_key"];
            }
            if (api_secret.empty() && j.contains("api_secret")) {
                api_secret = j["api_secret"];
            }
            
            // Load other settings
            if (j.contains("settings")) {
                for (const auto& [key, value] : j["settings"].items()) {
                    settings[key] = value;
                }
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "Warning: Error reading config file: " << e.what() << std::endl;
    }
}

Config::Config() {
    // Load configuration from environment variables first
    loadEnvironmentVariables();
    
    // Then load from config file (may override some values)
    loadConfigFile();
    
    // Set default values if not set
    if (settings.find("base_url") == settings.end()) {
        settings["base_url"] = "https://testnet.binance.vision";
    }
    
    if (!isValid()) {
        std::cerr << "Warning: API credentials not found in environment variables or config file" << std::endl;
    }
}

Config& Config::getInstance() {
    if (instance == nullptr) {
        instance = new Config();
    }
    return *instance;
}

std::string Config::getSetting(const std::string& key, const std::string& defaultValue) const {
    auto it = settings.find(key);
    return it != settings.end() ? it->second : defaultValue;
} 