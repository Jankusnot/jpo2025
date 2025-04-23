/**
 * @file NetworkingAndFileHandling.cpp
 * @brief Implementation of networking and file handling functionality
 */

#include "NetworkingAndFileHandling.h"

 /**
  * @brief Formats a date string into a filename-friendly format (YYYY-MM-DD_HH)
  * @param dateStr A date string in ISO format
  * @return A formatted string suitable for filenames (e.g., "2025-04-22_15")
  *
  * Extracts the date part and hour from an ISO format date string and combines them
  * with an underscore to create a filename-friendly format.
  */
std::string FormatDateForFilename(const std::string& dateStr) {
    // Check if date string has sufficient length
    if (dateStr.length() < 13) {
        return "unknown";
    }
    // Extract date part and hour, separated by underscore
    std::string formatted = dateStr.substr(0, 10) + "_" + dateStr.substr(11, 2);
    return formatted;
}

/**
 * @brief Callback function for CURL to handle received data by appending to a string
 * @param contents Pointer to the received data
 * @param size Size of each data element
 * @param nmemb Number of data elements
 * @param response Pointer to string where data will be stored
 * @return Total size of received data
 *
 * This function is called by libcurl when data is received during an HTTP request.
 * It appends the received data to the provided string.
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t newLength = size * nmemb;
    response->append((char*)contents, newLength);
    return newLength;
}

/**
 * @brief Performs an HTTP GET request to the specified URL and returns parsed JSON
 * @param url The URL to request data from
 * @return Json::Value object containing the parsed JSON response
 * @throw std::runtime_error if the request fails or JSON parsing fails
 *
 * Uses libcurl to perform an HTTP GET request and parses the response as JSON.
 */
Json::Value PerformHttpGet(const std::string& url) {
    // Initialize CURL session
    CURL* curl = curl_easy_init();
    Json::Value result;

    // Check if CURL initialized successfully
    if (!curl) {
        curl_easy_cleanup(curl);
        throw std::runtime_error("Failed to initialize CURL");
    }

    std::string readBuffer;
    // Configure CURL session options
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

    // Perform the HTTP request
    CURLcode res = curl_easy_perform(curl);
    // Clean up CURL resources
    curl_easy_cleanup(curl);

    // Check if request was successful
    if (res != CURLE_OK) {
        throw std::runtime_error(std::string("CURL request failed: ") + curl_easy_strerror(res));
    }

    // Parse JSON response
    Json::CharReaderBuilder builder;
    std::string errors;
    std::istringstream jsonStream(readBuffer);
    bool parsingSuccessful = Json::parseFromStream(builder, jsonStream, &result, &errors);

    // Check if JSON parsing was successful
    if (!parsingSuccessful) {
        throw std::runtime_error("Failed to parse JSON: " + errors);
    }

    return result;
}

/**
 * @brief Saves JSON data to a file with indentation
 * @param data Json::Value object to save
 * @param filePath Path where the file will be saved
 * @return true if successful, false otherwise
 * @throw std::runtime_error if the file cannot be opened for writing
 *
 * Writes JSON data to a file with proper indentation for readability.
 */
bool SaveToFile(Json::Value& data, const std::string& filePath) {
    // Open file for writing
    std::ofstream file(filePath);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for writing: " + filePath);
    }

    // Configure JSON writer with indentation and save to a file
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    writer->write(data, &file);
    file.close();
    return true;
}

/**
 * @brief Downloads JSON from URL, optionally transforms it, and saves to file
 * @param url The URL to download JSON from
 * @param filePath Path where the file will be saved
 * @param transformer Optional function to transform the JSON before saving
 * @return true if download and save were successful, false otherwise
 *
 * Fetches JSON data from a URL, applies an optional transformation function,
 * and saves the result to a file.
 */
bool DownloadJsonAndSaveToFile(const std::string& url, const std::string& filePath, std::function<Json::Value(const Json::Value&)> transformer) {
    try {
        // Perform HTTP GET request
        Json::Value root = PerformHttpGet(url);

        // Apply transformer function if provided
        root = transformer ? transformer(root) : root;

        // Save transformed JSON to file
        return SaveToFile(root, filePath);
    }
    catch (const std::exception& e) {
        // Display error message if download or save fails
        wxMessageBox(wxString::Format("Error downloading and saving data: %s", e.what()),
            "Download Error", wxOK | wxICON_ERROR);
        return false;
    }
}

/**
 * @brief Downloads air quality data, filters valid entries, and saves to a file with date-based name
 * @param url The URL to download air quality data from
 * @param dirPath Directory path where the file will be saved
 * @return true if download and save were successful, false otherwise
 *
 * Downloads air quality data in JSON format, filters out invalid entries,
 * creates a filename based on the date range of the data, and saves the
 * filtered data to the specified directory.
 */
bool DownloadJsonAndSaveData(const std::string& url, const std::string& dirPath) {
    try {
        // Perform HTTP GET request
        Json::Value root = PerformHttpGet(url);

        const Json::Value& values = root["values"];
        Json::Value filteredValues(Json::arrayValue);
        std::string firstValidDate;
        std::string lastValidDate;

        // Loop through values and keep only valid entries
        for (Json::Value::ArrayIndex i = 0; i < values.size(); i++) {

            const Json::Value& item = values[i];
            // Check if item has required fields
            if (item.isMember("date") && item.isMember("value") && !item["value"].isNull()) {
                filteredValues.append(item);

                std::string currentDate = item["date"].asString();

                if (firstValidDate.empty()) {
                    firstValidDate = currentDate;
                }
                lastValidDate = currentDate;
            }
        }

        // Verify we have at least one valid data point
        if (filteredValues.size() == 0) {
            throw std::runtime_error("There are no data point in the file.");
        }

        // Format dates for filename
        std::string firstDateForFilename = FormatDateForFilename(firstValidDate);
        std::string lastDateForFilename = FormatDateForFilename(lastValidDate);

        // Create filename with date range
        std::string filename = lastDateForFilename + "_to_" + firstDateForFilename + ".json";
        std::string filePath = dirPath + "/" + filename;

        // Prepare result JSON structure
        Json::Value result;
        if (root.isMember("key")) {
            result["key"] = root["key"];
        }
        result["values"] = filteredValues;

        // Save filtered data to file
        return SaveToFile(result, filePath);
    }
    catch (const std::exception& e) {
        // Display error message if download fails
        wxMessageBox(wxString::Format("Error downloading data: %s", e.what()),
            "Download Error", wxOK | wxICON_ERROR);
        return false;
    }
}

/**
 * @brief Loads and parses JSON data from a file into the provided output value
 * @param filePath Path of the file to load
 * @param output Json::Value object where parsed data will be stored
 * @return true if file loading and parsing were successful, false otherwise
 *
 * Opens a file, reads its contents, and parses it as JSON into the provided output parameter.
 */
bool LoadJsonFromFile(const std::string& filePath, Json::Value& output) {
    // Open file for reading
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    // Parse JSON from file
    Json::CharReaderBuilder builder;
    std::string errors;
    bool parsingSuccessful = Json::parseFromStream(builder, file, &output, &errors);
    file.close();

    return parsingSuccessful;
}