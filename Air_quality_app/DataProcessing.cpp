/**
 * @file DataProcessing.cpp
 * @brief Implementation of data processing functions for JSON time series data
 */

#include "DataProcessing.h"

 /**
  * @brief Processes JSON data from a file and updates the info and chart panels with the results
  *
  * This function:
  * 1. Loads and parses a JSON file
  * 2. Validates the JSON structure for required elements
  * 3. Extracts time series data points
  * 4. Calculates statistics (min, max, average, trend)
  * 5. Updates the UI components with processed data
  *
  * @param filePath Path to the JSON file containing the data
  * @param infoPanel Pointer to the InfoPanel to update with statistics (can be nullptr)
  * @param chartPanel Pointer to the ChartPanel to update with chart data (can be nullptr)
  */
void ProcessFileData(const std::string& filePath, InfoPanel* infoPanel, ChartPanel* chartPanel) {
    Json::Value data;
    // Check if file can be opened and parsed
    if (!LoadJsonFromFile(filePath, data)) {
        wxMessageBox("Could not open or parse data file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Validate that the JSON contains a values array
    if (!data.isMember("values") || !data["values"].isArray()) {
        wxMessageBox("Failed to parse data file: missing values array.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string key = data.isMember("key") ? data["key"].asString() : "Unknown";
    std::string paramCode = data.isMember("param") ? data["param"].asString() : "";

    const Json::Value& values = data["values"];
    // Check if values array is empty
    if (values.size() == 0) {
        wxMessageBox("No values found in the data file.", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    // Initialize variables for statistical calculations
    double max = std::numeric_limits<double>::lowest();
    double min = std::numeric_limits<double>::max();
    double sum = 0.0;
    int count = 0;
    // Create vectors to store time series data for analysis and chart display
    std::vector<std::pair<std::string, double>> timeSeriesData;
    std::vector<std::pair<wxString, double>> chartData;

    // Scan values array for valid data points
    for (Json::Value::ArrayIndex i = 0; i < values.size(); i++) {
        const Json::Value& item = values[i];
        if (item.isMember("value") && !item["value"].isNull() && item.isMember("date")) {
            double value = item["value"].asDouble();
            std::string dateStr = item["date"].asString();

            max = std::max(max, value);
            min = std::min(min, value);
            sum += value;
            count++;

            timeSeriesData.push_back(std::make_pair(dateStr, value));
            chartData.push_back(std::make_pair(wxString(dateStr), value));
        }
    }

    // Sort chart data by date
    std::sort(chartData.begin(), chartData.end(),
        [](const std::pair<wxString, double>& a, const std::pair<wxString, double>& b) {
            return a.first < b.first;
        });

    double average = (count > 0) ? (sum / count) : 0.0;

    // Calculate trend
    std::string trend = CalculateTrend(timeSeriesData);

    // Update info panel with calculated statistics if available
    if (infoPanel) {
        infoPanel->UpdateStatistics(key, max, min, average, trend);
    }

    // Prepare parameters for ChartPanel
    wxString chartTitle = wxString::Format("Air Quality Measurements: %s", key);
    wxString yAxisLabel = paramCode;

    // If parameter code is empty, try to extract from key format "Name (Code)"
    if (yAxisLabel.IsEmpty() && !key.empty()) {
        size_t pos = key.find('(');
        if (pos != std::string::npos) {
            size_t end = key.find(')', pos);
            if (end != std::string::npos) {
                // Extract text between parentheses for y-axis label
                yAxisLabel = key.substr(pos + 1, end - pos - 1);
            }
        }

        // Use "Value" as fallback label if nothing else is available
        if (yAxisLabel.IsEmpty()) {
            yAxisLabel = "Value";
        }
    }

    // Update chart panel with processed data if available
    if (chartPanel && !chartData.empty()) {
        chartPanel->SetChartData(chartData, chartTitle, "Date", yAxisLabel);
    }
}

/**
 * @brief Analyzes time series data to determine and describe the trend direction and magnitude
 *
 * Uses linear regression to calculate the slope of the time series data and
 * categorizes the trend based on the percentage change relative to the average value.
 *
 * The function returns one of the following trend descriptions:
 * - "insufficient data for trend analysis": When there are fewer than 2 data points
 * - "stable": When the percentage change is less than 5%
 * - "increasing": When the percentage change is between 5% and 20%
 * - "strongly increasing": When the percentage change is greater than 20%
 * - "decreasing": When the percentage change is between -5% and -20%
 * - "strongly decreasing": When the percentage change is less than -20%
 *
 * @param data Vector of date-value pairs representing time series data
 * @return std::string Description of the trend
 */
std::string CalculateTrend(const std::vector<std::pair<std::string, double>>& data) {
    if (data.size() < 2) {
        return "insufficient data for trend analysis";
    }

    // Create a sorted copy of the data by date
    std::vector<std::pair<std::string, double>> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end(),
        [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
            return a.first < b.first;
        });

    int n = sortedData.size();

    // Initialize variables for linear regression calculation
    double sumX = 0.0;
    double sumY = 0.0;
    double sumXY = 0.0;
    double sumX2 = 0.0;

    // Trend calculations
    for (int i = 0; i < n; i++) {
        sumX += i;
        sumY += sortedData[i].second;
        sumXY += i * sortedData[i].second;
        sumX2 += i * i;
    }

    double slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    double average = sumY / n;
    double totalChange = slope * (n - 1);
    double percentChange = (average != 0) ? (totalChange / average) * 100.0 : 0.0;

    // Determine trend description based on percent change thresholds
    if (std::abs(percentChange) < 5.0) {
        return "stable";
    }
    else if (percentChange > 0) {
        if (percentChange > 20.0) {
            return "strongly increasing";
        }
        else {
            return "increasing";
        }
    }
    else {
        if (percentChange < -20.0) {
            return "strongly decreasing";
        }
        else {
            return "decreasing";
        }
    }
}