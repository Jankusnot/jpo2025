#include "NetworkingAndFileHandling.h"

// Formats a date string into a filename-friendly format (YYYY-MM-DD_HH)
std::string FormatDateForFilename(const std::string& dateStr) {
    // Check if date string has sufficient length
    if (dateStr.length() < 13) {
        return "unknown";
    }
    // Extract date part and hour, separated by underscore
    std::string formatted = dateStr.substr(0, 10) + "_" + dateStr.substr(11, 2);
    return formatted;
}

// Callback function for CURL to handle received data by appending to a string
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t newLength = size * nmemb;
    response->append((char*)contents, newLength);
    return newLength;
}

// Performs an HTTP GET request to the specified URL and returns parsed JSON
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

// Saves JSON data to a file with indentation
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

// Downloads JSON from URL, optionally transforms it, and saves to file
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

// Downloads air quality data, filters valid entries, and saves to a file with date-based name
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

// Loads and parses JSON data from a file into the provided output value
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