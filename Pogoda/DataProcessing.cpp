#include "DataProcessing.h"

void ProcessFileData(const std::string& filePath, InfoPanel* infoPanel, ChartPanel* chartPanel) {
    Json::Value data;
    if (!LoadJsonFromFile(filePath, data)) {
        wxMessageBox("Could not open or parse data file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    if (!data.isMember("values") || !data["values"].isArray()) {
        wxMessageBox("Failed to parse data file: missing values array.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    std::string key = data.isMember("key") ? data["key"].asString() : "Unknown";
    std::string paramCode = data.isMember("param") ? data["param"].asString() : "";

    const Json::Value& values = data["values"];
    if (values.size() == 0) {
        wxMessageBox("No values found in the data file.", "Warning", wxOK | wxICON_WARNING);
        return;
    }

    double max = std::numeric_limits<double>::lowest();
    double min = std::numeric_limits<double>::max();
    double sum = 0.0;
    int count = 0;
    std::vector<std::pair<std::string, double>> timeSeriesData;
    std::vector<std::pair<wxString, double>> chartData;

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

    std::sort(chartData.begin(), chartData.end(),
        [](const std::pair<wxString, double>& a, const std::pair<wxString, double>& b) {
            return a.first < b.first;
        });

    double average = (count > 0) ? (sum / count) : 0.0;

    std::string trend = CalculateTrend(timeSeriesData);

    if (infoPanel) {
        infoPanel->UpdateStatistics(key, max, min, average, trend);
    }

    wxString chartTitle = wxString::Format("Air Quality Measurements: %s", key);
    wxString yAxisLabel = paramCode;

    if (yAxisLabel.IsEmpty() && !key.empty()) {
        size_t pos = key.find('(');
        if (pos != std::string::npos) {
            size_t end = key.find(')', pos);
            if (end != std::string::npos) {
                yAxisLabel = key.substr(pos + 1, end - pos - 1);
            }
        }

        if (yAxisLabel.IsEmpty()) {
            yAxisLabel = "Value";
        }
    }

    if (chartPanel && !chartData.empty()) {
        chartPanel->SetChartData(chartData, chartTitle, "Date", yAxisLabel);
    }
}

std::string CalculateTrend(const std::vector<std::pair<std::string, double>>& data) {
    if (data.size() < 2) {
        return "insufficient data for trend analysis";
    }

    std::vector<std::pair<std::string, double>> sortedData = data;
    std::sort(sortedData.begin(), sortedData.end(),
        [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
            return a.first < b.first;
        });

    int n = sortedData.size();
    int firstThirdEnd = n / 3;
    int lastThirdStart = 2 * n / 3;

    double firstThirdSum = 0.0;
    for (int i = 0; i < firstThirdEnd; i++) {
        firstThirdSum += sortedData[i].second;
    }
    double firstThirdAvg = firstThirdSum / firstThirdEnd;

    double lastThirdSum = 0.0;
    for (int i = lastThirdStart; i < n; i++) {
        lastThirdSum += sortedData[i].second;
    }
    double lastThirdAvg = lastThirdSum / (n - lastThirdStart);

    double difference = lastThirdAvg - firstThirdAvg;
    double percentChange = (firstThirdAvg != 0) ? (difference / firstThirdAvg) * 100.0 : 0.0;

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