#ifndef NETWORKING_AND_FILE_HANDLING_H
#define NETWORKING_AND_FILE_HANDLING_H

#define _CRT_SECURE_NO_WARNINGS

#include <wx/wx.h>
#include <curl/curl.h>
#include <json/json.h>
#include <fstream>
#include <string>

/**
 * @file NetworkingAndFileHandling.h
 * @brief Header file defining networking and file handling functionality
 */

 /**
  * @brief Formats a date string into a filename-friendly format
  * @param dateStr A date string in ISO format
  * @return A formatted string suitable for filenames (e.g., "2025-04-22_15")
  */
std::string FormatDateForFilename(const std::string& dateStr);

/**
 * @brief Callback function for CURL to handle received data
 * @param contents Pointer to the received data
 * @param size Size of each data element
 * @param nmemb Number of data elements
 * @param s Pointer to string where data will be stored
 * @return Total size of received data
 *
 * @note This function is called by libcurl when data is received during an HTTP request
 */
static size_t WriteCallback(void* contents, size_t size, size_t nmemb, std::string* s);

/**
 * @brief Performs an HTTP GET request to the specified URL
 * @param url The URL to request data from
 * @return Json::Value object containing the parsed JSON response
 * @throw std::runtime_error if the request fails or JSON parsing fails
 */
Json::Value PerformHttpGet(const std::string& url);

/**
 * @brief Saves JSON data to a file with indentation
 * @param data Json::Value object to save
 * @param filePath Path where the file will be saved
 * @return true if successful, false otherwise
 * @throw std::runtime_error if the file cannot be opened for writing
 */
bool SaveToFile(Json::Value& data, const std::string& filePath);

/**
 * @brief Downloads JSON from URL, optionally transforms it, and saves to file
 * @param url The URL to download JSON from
 * @param filePath Path where the file will be saved
 * @param transformer Optional function to transform the JSON before saving
 * @return true if download and save were successful, false otherwise
 */
bool DownloadJsonAndSaveToFile(const std::string& url, const std::string& filePath, std::function<Json::Value(const Json::Value&)> transformer = nullptr);

/**
 * @brief Downloads air quality data, filters valid entries, and saves to a file with date-based name
 * @param url The URL to download air quality data from
 * @param dirPath Directory path where the file will be saved
 * @return true if download and save were successful, false otherwise
 */
bool DownloadJsonAndSaveData(const std::string& url, const std::string& dirPath);

/**
 * @brief Loads and parses JSON data from a file
 * @param filePath Path of the file to load
 * @param output Json::Value object where parsed data will be stored
 * @return true if file loading and parsing were successful, false otherwise
 */
bool LoadJsonFromFile(const std::string& filePath, Json::Value& output);

#endif // NETWORKING_AND_FILE_HANDLING_H