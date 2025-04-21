#ifndef NETWORKING_AND_FILE_HANDLING_H
#define NETWORKING_AND_FILE_HANDLING_H

#define _CRT_SECURE_NO_WARNINGS

#include <wx/wx.h>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>
#include <string>

std::string FormatDateForFilename(const std::string& dateStr);
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
Json::Value PerformHttpGet(const std::string& url);
bool SaveToFile(Json::Value& data, const std::string& filePath);
bool DownloadJsonAndSaveToFile(const std::string& url, const std::string& filePath, std::function<Json::Value(const Json::Value&)> transformer = nullptr);
bool DownloadJsonAndSaveData(const std::string& url, const std::string& dirPath);
bool LoadJsonFromFile(const std::string& filePath, Json::Value& output);

#endif