#include "Networking.h"

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s) {
    size_t newLength = size * nmemb;
    s->append((char*)contents, newLength);
    return newLength;
}

bool PerformHttpGet(const std::string& url, std::string& outputBuffer) {
    CURL* curl = curl_easy_init();
    if (!curl) {
        return false;
    }

    CURLcode res;
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &outputBuffer);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, 10);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, 0L);

    res = curl_easy_perform(curl);
    curl_easy_cleanup(curl);

    return (res == CURLE_OK);
}

bool DownloadJsonAndSaveToFile(const std::string& url, const std::string& filePath,
    std::function<Json::Value(const Json::Value&)> transformer) {
    std::string readBuffer;
    if (!PerformHttpGet(url, readBuffer)) {
        return false;
    }

    Json::Value root;
    Json::CharReaderBuilder builder;
    std::string errors;
    std::istringstream jsonStream(readBuffer);

    bool parsingSuccessful = Json::parseFromStream(builder, jsonStream, &root, &errors);
    if (!parsingSuccessful) {
        return false;
    }

    Json::Value outputJson = transformer ? transformer(root) : root;

    std::ofstream file(filePath);
    if (!file.is_open()) {
        return false;
    }

    Json::StreamWriterBuilder writerBuilder;
    writerBuilder["indentation"] = "  ";
    std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
    writer->write(outputJson, &file);
    file.close();

    return true;
}