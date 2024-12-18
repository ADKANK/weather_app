#include <iostream>
#include <string>
#include <cstdlib>        
#include <curl/curl.h>    
#include "json.hpp"       
#include "dotenv.h"       

using json = nlohmann::json;

// Function to handle data received by libcurl
size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* output) {
    size_t totalSize = size * nmemb;
    output->append((char*)contents, totalSize);
    return totalSize;
}

// Function to fetch weather data
std::string getWeatherData(const std::string& city, const std::string& apiKey) {
    CURL* curl;
    CURLcode res;
    std::string readBuffer;

    // OpenWeatherMap API endpoint
    std::string url = "http://api.openweathermap.org/data/2.5/weather?q=" + city + "&appid=" + apiKey + "&units=metric";

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        res = curl_easy_perform(curl);

        if (res != CURLE_OK) {
            std::cerr << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
        }

        curl_easy_cleanup(curl);
    }
    return readBuffer;
}

int main() {
    dotenv::init(); // Load .env file

    const char* apiKey = std::getenv("WEATHER_API_KEY");
    if (!apiKey) {
        std::cerr << "Error: WEATHER_API_KEY not found in .env file." << std::endl;
        return 1;
    }

    std::string city;
    std::cout << "Enter the city name: ";
    std::getline(std::cin, city);


    std::string weatherData = getWeatherData(city, apiKey);

 
    try {
        auto jsonData = json::parse(weatherData);

        // Extract weather details
        std::cout << "\nWeather in " << city << ":\n";
        std::cout << "Temperature: " << jsonData["main"]["temp"] << " °C\n";
        std::cout << "Humidity: " << jsonData["main"]["humidity"] << " %\n";
        std::cout << "Weather: " << jsonData["weather"][0]["description"] << std::endl;
    } catch (json::parse_error& e) {
        std::cerr << "Error parsing weather data: " << e.what() << std::endl;
    }

    return 0;
}
