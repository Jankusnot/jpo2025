#ifndef NETWORKING_H
#define NETWORKING_H

#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include <wx/msgdlg.h>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <vector>

static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);
bool PerformHttpGet(const std::string& url, std::string& outputBuffer);
bool DownloadJsonAndSaveToFile(const std::string& url, const std::string& filePath, std::function<Json::Value(const Json::Value&)> transformer = nullptr);

#endif