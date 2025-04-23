#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <wx/wx.h>
#include <Json/json.h>
#include <algorithm>
#include <string>
#include <vector>

#include "InfoPanel.h"
#include "ChartPanel.h"

#include "NetworkingAndFileHandling.h"

/**
 * @file DataProcessing.h
 * @brief Functions for processing data from JSON files and calculating statistics
 */

 /**
  * @brief Processes JSON data from a file and updates UI components
  *
  * This function loads a JSON file, extracts time series data, calculates statistics,
  * and updates the provided info and chart panels with the results.
  *
  * @param filePath Path to the JSON file containing the data
  * @param infoPanel Pointer to the InfoPanel to update with statistics (can be nullptr)
  * @param chartPanel Pointer to the ChartPanel to update with chart data (can be nullptr)
  */
void ProcessFileData(const std::string& filePath, InfoPanel* infoPanel, ChartPanel* chartPanel);

/**
 * @brief Analyzes time series data to determine the trend
 *
 * Performs linear regression on the time series data to calculate the slope
 * and describes the trend as "stable", "increasing", "strongly increasing",
 * "decreasing", or "strongly decreasing".
 *
 * @param data Vector of date-value pairs representing time series data
 * @return std::string Description of the trend
 */
std::string CalculateTrend(const std::vector<std::pair<std::string, double>>& data);

#endif // DATA_PROCESSING_H