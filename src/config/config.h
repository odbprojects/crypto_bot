#pragma once
#include <string>
#include <map>

class Config {
private:
    std::string api_key;
    std::string api_secret;
    std::map<std::string, std::string> settings;
    static Config* instance;
    
    // Private constructor for singleton pattern
    Config();
    
    // Helper methods
    void loadEnvironmentVariables();
    void loadConfigFile();
    std::string getEnvVar(const std::string& key);

public:
    // Delete copy constructor and assignment operator
    Config(const Config&) = delete;
    Config& operator=(const Config&) = delete;

    // Get singleton instance
    static Config& getInstance();

    // Getters for API credentials
    const std::string& getApiKey() const { return api_key; }
    const std::string& getApiSecret() const { return api_secret; }
    
    // Get other configuration settings
    std::string getSetting(const std::string& key, const std::string& defaultValue = "") const;
    
    // Check if configuration is valid
    bool isValid() const { return !api_key.empty() && !api_secret.empty(); }
    
    // Cleanup
    static void cleanup() {
        delete instance;
        instance = nullptr;
    }
}; 