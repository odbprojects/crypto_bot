#include <iostream>
#include <openssl/hmac.h>
#include <openssl/sha.h>
#include <iomanip>
#include <sstream>
#include <curl/curl.h>
#include "api.h"
#include "config/config.h"

// Define the static member function
size_t BinanceAPI::WriteCallback(void* contents, size_t size, size_t nmemb, std::string* userp) {
    userp->append((char*)contents, size * nmemb);
    return size * nmemb;
}

// Generate HMAC-SHA256 signature for API request authentication
std::string BinanceAPI::hmac_sha256(const std::string &key, const std::string &data) {
    unsigned char* digest;
    digest = HMAC(EVP_sha256(), key.c_str(), key.length(), (unsigned char*)data.c_str(), data.length(), NULL, NULL);
    if (!digest) {
        std::cerr << "HMAC calculation failed!" << std::endl;
        return "";
    }
    
    std::ostringstream oss;
    for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
        oss << std::hex << std::setw(2) << std::setfill('0') << (int)digest[i];
    }
    return oss.str();
}

// Send authenticated request to Binance API
std::string BinanceAPI::send_signed_request(const std::string &endpoint, const std::string &query, const std::string &method) {
    // Get current timestamp in milliseconds using chrono
    auto now = std::chrono::system_clock::now();
    auto millis = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
    
    // Build the query string carefully
    std::string params = query;
    if (!params.empty() && params.back() != '&') {
        params += '&';
    }
    params += "timestamp=" + std::to_string(millis);
    
    // Calculate signature
    std::string signature = hmac_sha256(config.getApiSecret(), params);
    
    // Build final query with signature
    std::string final_params = params + "&signature=" + signature;
    
    std::string url = config.getSetting("base_url") + endpoint;
    
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    // Set up headers
    struct curl_slist* headers = NULL;
    headers = curl_slist_append(headers, ("X-MBX-APIKEY: " + config.getApiKey()).c_str());
    
    std::string response;
    
    curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1);
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
    
    if (method == "POST") {
        // For POST requests
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        
        // Use COPYPOSTFIELDS and append parameters to URL instead
        std::string full_url = url + "?" + final_params;
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, "");  // Empty POST body
        
        // Minimal debug output
        std::cout << "Sending order to: " << url << std::endl;
    } else {
        // For GET requests
        std::string full_url = url + "?" + final_params;
        curl_easy_setopt(curl, CURLOPT_URL, full_url.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }

    // Execute request
    CURLcode res = curl_easy_perform(curl);
    
    // Get HTTP response code and headers
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    
    // Print response headers for debugging
    if (http_code >= 400) {
        char* content_type;
        curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &content_type);
        std::cerr << "HTTP error " << http_code << std::endl;
        std::cerr << "Content-Type: " << (content_type ? content_type : "unknown") << std::endl;
        std::cerr << "Response: " << response << std::endl;
    }
    
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);
    
    if (res != CURLE_OK) {
        std::cerr << "Failed to execute request: " << curl_easy_strerror(res) << std::endl;
        return "";
    }

    return response;
}

// Send unauthenticated request to Binance API
std::string BinanceAPI::send_public_request(const std::string &endpoint) {
    CURL* curl = curl_easy_init();
    if (!curl) return "";

    std::string url = config.getSetting("base_url") + endpoint;
    std::string response;
    
    // Configure CURL options
    CURLcode code = CURLE_OK;
    int timeout = std::stoi(config.getSetting("timeout", "30"));
    if ((code = curl_easy_setopt(curl, CURLOPT_HTTP_VERSION, CURL_HTTP_VERSION_1_1)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_URL, url.c_str())) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 1L)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 2L)) != CURLE_OK ||
        (code = curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L)) != CURLE_OK) {
        curl_easy_cleanup(curl);
        return "";
    }

    // Execute request with retry logic
    int retry_attempts = std::stoi(config.getSetting("retry_attempts", "3"));
    int retry_delay = std::stoi(config.getSetting("retry_delay", "1000"));
    
    CURLcode res;
    for (int i = 0; i < retry_attempts; i++) {
        res = curl_easy_perform(curl);
        if (res == CURLE_OK) break;
        if (i < retry_attempts - 1) {
            std::cerr << "Request failed, retrying in " << retry_delay << "ms..." << std::endl;
            std::this_thread::sleep_for(std::chrono::milliseconds(retry_delay));
        }
    }

    curl_easy_cleanup(curl);
    
    return (res == CURLE_OK) ? response : "";
}
