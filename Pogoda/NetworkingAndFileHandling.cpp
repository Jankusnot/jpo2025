#include "NetworkingAndFileHandling.h"

std::string FormatDateForFilename(const std::string& dateStr) {
    if (dateStr.length() < 13) {
        return "unknown";
    }

    std::string formatted = dateStr.substr(0, 10) + "_" + dateStr.substr(11, 2);
    return formatted;
}

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* response) {
    size_t newLength = size * nmemb;
    response->append((char*)contents, newLength);
    return newLength;
}

Json::Value PerformHttpGet(const std::string& url) {
    CURL* curl = curl_easy_init();
    Json::Value result;

    if (!curl) {
        throw std::runtime_error("Failed to initialize CURL");
    }

    try {
        std::string readBuffer;
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10L);

        CURLcode res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res != CURLE_OK) {
            throw std::runtime_error(std::string("CURL request failed: ") + curl_easy_strerror(res));
        }

        Json::CharReaderBuilder builder;
        std::string errors;
        std::istringstream jsonStream(readBuffer);
        bool parsingSuccessful = Json::parseFromStream(builder, jsonStream, &result, &errors);

        if (!parsingSuccessful) {
            throw std::runtime_error("Failed to parse JSON: " + errors);
        }

        return result;
    }
    catch (const std::exception& e) {
        curl_easy_cleanup(curl);
        throw;
    }
}

bool SaveToFile(Json::Value& data, std::ofstream& file) {
    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    writer->write(data, &file);
    file.close();
    return true;
}

bool DownloadJsonAndSaveToFile(const std::string& url, const std::string& filePath, std::function<Json::Value(const Json::Value&)> transformer) {
    try {
        Json::Value root = PerformHttpGet(url);

        root = transformer ? transformer(root) : root;

        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filePath);
        }

        return SaveToFile(root, file);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error downloading and saving data: %s", e.what()),
            "Download Error", wxOK | wxICON_ERROR);
        return false;
    }
}

bool DownloadJsonAndSaveData(const std::string& url, const std::string& dirPath) {
    try {
        Json::Value root = PerformHttpGet(url);

        const Json::Value& values = root["values"];
        
        Json::Value filteredValues(Json::arrayValue);
        std::string firstValidDate;
        std::string lastValidDate;
        
        for (Json::Value::ArrayIndex i = 0; i < values.size(); i++) {
        
            const Json::Value& item = values[i];
            if (item.isMember("date") && item.isMember("value") && !item["value"].isNull()) {
                filteredValues.append(item);
        
                std::string currentDate = item["date"].asString();
        
                if (firstValidDate.empty()) {
                    firstValidDate = currentDate;
                }
        
                lastValidDate = currentDate;
            }
        }
        
        if (filteredValues.size() == 0) {
            throw std::runtime_error("There are no data point in the file.");
        }
        
        std::string firstDateForFilename = FormatDateForFilename(firstValidDate);
        std::string lastDateForFilename = FormatDateForFilename(lastValidDate);
        
        std::string filename = lastDateForFilename + "_to_" + firstDateForFilename + ".json";
        std::string filePath = dirPath + "/" + filename;
        
        Json::Value result;
        if (root.isMember("key")) {
            result["key"] = root["key"];
        }
        result["values"] = filteredValues;
        
        std::ofstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Could not open file for writing: " + filePath);
        }

        return SaveToFile(result, file);
    }
    catch (const std::exception& e) {
        wxMessageBox(wxString::Format("Error downloading data: %s", e.what()),
            "Download Error", wxOK | wxICON_ERROR);
        return false;
    }
}

bool LoadJsonFromFile(const std::string& filePath, Json::Value& output) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    Json::CharReaderBuilder builder;
    std::string errors;
    bool parsingSuccessful = Json::parseFromStream(builder, file, &output, &errors);
    file.close();

    return parsingSuccessful;
}