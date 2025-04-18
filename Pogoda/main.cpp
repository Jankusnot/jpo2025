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

#include "StationSelectionDialog.h"
#include "SensorSelectionDialog.h"
#include "FileSelectionDialog.h"

#include "ChartPanel.h"
#include "InfoPanel.h"
#include "SidePanel.h"

#include "NetworkingAndFileHandling.h"





//class SidePanell : public wxPanel {
//private:
//    // UI Components
//    wxButton* btn1;
//    wxButton* btn2;
//    wxButton* btn3;
//
//    wxButton* btn4;
//
//    wxStaticText* selectedStationText;
//    wxStaticText* selectedSensorText;
//    wxStaticText* selectedFileText;
//
//    // Selection state
//    int selectedStationId;
//    wxString selectedStationName;
//    int selectedSensorId;
//    wxString selectedSensorParamName;
//    wxString selectedFileName;
//
//    // Panel references
//    InfoPanel* infoPanel;
//    ChartPanel* chartPanel;
//
//    // Constants
//    const std::string STATIONS_API_URL = "http://api.gios.gov.pl/pjp-api/rest/station/findAll";
//    const std::string STATIONS_FILE_PATH = "pomiary/stacje.json";
//
//public:
//    SidePanel(wxWindow* parent)
//        : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 800)) {
//
//        SetBackgroundColour(wxColour(220, 220, 220));
//
//        InitializeUI();
//        BindEvents();
//
//        // Initialize selection state
//        selectedStationId = -1;
//        selectedStationName = "";
//        selectedSensorId = -1;
//        selectedSensorParamName = "";
//        selectedFileName = "";
//    }
//
//    void ConnectToPanels(InfoPanel* info, ChartPanel* chart) {
//        infoPanel = info;
//        chartPanel = chart;
//    }
//
//private:
//    // UI initialization
//    void InitializeUI() {
//        wxBoxSizer* btnSizer = new wxBoxSizer(wxVERTICAL);
//
//        btn1 = new wxButton(this, wxID_ANY, "Chose station");
//        btn2 = new wxButton(this, wxID_ANY, "Chose sensor");
//        btn3 = new wxButton(this, wxID_ANY, "Select data");
//        
//        btn4 = new wxButton(this, wxID_ANY, "View downloaded data");
//        
//        
//
//        // Set button fonts
//        wxFont buttonFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);
//        btn1->SetFont(buttonFont);
//        btn2->SetFont(buttonFont);
//        btn3->SetFont(buttonFont);
//
//        btn4->SetFont(buttonFont);
//
//        // Initialize status text controls
//        wxFont statusFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);
//
//        selectedStationText = new wxStaticText(this, wxID_ANY, "No station selected");
//        selectedStationText->SetFont(statusFont);
//
//        selectedSensorText = new wxStaticText(this, wxID_ANY, "No sensor selected");
//        selectedSensorText->SetFont(statusFont);
//
//        selectedFileText = new wxStaticText(this, wxID_ANY, "No file selected");
//        selectedFileText->SetFont(statusFont);
//
//        // Add components to sizer
//        btnSizer->Add(btn1, 0, wxALL | wxEXPAND, 10);
//        btnSizer->Add(btn2, 0, wxALL | wxEXPAND, 10);
//        btnSizer->Add(btn3, 0, wxALL | wxEXPAND, 10);
//
//        btnSizer->Add(btn4, 0, wxALL | wxEXPAND, 10);
//
//        btnSizer->Add(selectedStationText, 0, wxALL | wxEXPAND, 10);
//        btnSizer->Add(selectedSensorText, 0, wxALL | wxEXPAND, 10);
//        btnSizer->Add(selectedFileText, 0, wxALL | wxEXPAND, 10);
//
//        SetSizer(btnSizer);
//    }
//
//    void BindEvents() {
//        btn1->Bind(wxEVT_BUTTON, &SidePanel::OnChoseStation, this);
//        btn2->Bind(wxEVT_BUTTON, &SidePanel::OnChooseSensor, this);
//        btn3->Bind(wxEVT_BUTTON, &SidePanel::OnShowDataFiles, this);
//
//        btn4->Bind(wxEVT_BUTTON, &SidePanel::OnViewDownloadedData, this);
//    }
//
//    // Status display updates
//    void UpdateSelectedStationDisplay() {
//        if (selectedStationId != -1) {
//            wxString info = wxString::Format("Selected station:\n%s (ID: %d)",
//                selectedStationName, selectedStationId);
//            selectedStationText->SetLabel(info);
//        }
//        else {
//            selectedStationText->SetLabel("No station selected");
//        }
//    }
//
//    void UpdateSelectedSensorDisplay() {
//        if (selectedSensorId != -1) {
//            wxString info = wxString::Format("Selected sensor:\n%s (ID: %d)",
//                selectedSensorParamName, selectedSensorId);
//            selectedSensorText->SetLabel(info);
//        }
//        else {
//            selectedSensorText->SetLabel("No sensor selected");
//        }
//    }
//
//    void UpdateSelectedFileDisplay() {
//        if (!selectedFileName.IsEmpty()) {
//            wxString info = wxString::Format("Selected file:\n%s", selectedFileName);
//            selectedFileText->SetLabel(info);
//        }
//        else {
//            selectedFileText->SetLabel("No file selected");
//        }
//    }
//
//    //Button binded functions
//    void OnChoseStation(wxCommandEvent& event) {
//        std::filesystem::create_directories("pomiary");
//
//        if (!std::filesystem::exists(STATIONS_FILE_PATH)) {
//
//            auto stationTransformer = [](const Json::Value& root) {
//                Json::Value filteredStations(Json::arrayValue);
//
//                for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
//                    const Json::Value& station = root[i];
//                    Json::Value filteredStation;
//
//                    if (station.isMember("id"))
//                        filteredStation["id"] = station["id"];
//
//                    if (station.isMember("stationName"))
//                        filteredStation["stationName"] = station["stationName"];
//
//                    if (station.isMember("gegrLat"))
//                        filteredStation["gegrLat"] = station["gegrLat"];
//
//                    if (station.isMember("gegrLon"))
//                        filteredStation["gegrLon"] = station["gegrLon"];
//
//                    filteredStations.append(filteredStation);
//                }
//                return filteredStations;
//                };
//
//            bool success = DownloadJsonAndSaveToFile(STATIONS_API_URL, STATIONS_FILE_PATH, stationTransformer);
//            if (!success) {
//                wxMessageBox("Failed to download stations data. Please check your internet connection and try again.", "Error", wxOK | wxICON_ERROR);
//                return;
//            }
//        }
//
//        Json::Value stations;
//        if (!LoadJsonFromFile(STATIONS_FILE_PATH, stations)) {
//            wxMessageBox("Could not open or parse stations file.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//
//        StationSelectionDialog dialog(this, stations);
//        if (dialog.ShowModal() == wxID_OK) {
//            int id;
//            wxString name;
//            if (dialog.GetSelectedStation(id, name)) {
//                selectedStationId = id;
//                selectedStationName = name;
//                UpdateSelectedStationDisplay();
//
//                selectedSensorId = -1;
//                selectedSensorParamName = "";
//                UpdateSelectedSensorDisplay();
//            }
//        }
//    }
//
//    void OnChooseSensor(wxCommandEvent& event) {
//        if (selectedStationId == -1) {
//            wxMessageBox("Please select a station first.", "No Station Selected", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        std::string stationDirPath = "pomiary/" + std::to_string(selectedStationId);
//        std::string sensorsFilePath = stationDirPath + "/sensors.json";
//
//        std::filesystem::create_directories(stationDirPath);
//
//        if (!std::filesystem::exists(sensorsFilePath)) {
//            std::string apiUrl = "http://api.gios.gov.pl/pjp-api/rest/station/sensors/" + std::to_string(selectedStationId);
//
//            if (!DownloadJsonAndSaveToFile(apiUrl, sensorsFilePath)) {
//                wxMessageBox("Failed to download sensors data.", "Error", wxOK | wxICON_ERROR);
//                return;
//            }
//        }
//
//        Json::Value sensors;
//        if (!LoadJsonFromFile(sensorsFilePath, sensors)) {
//            wxMessageBox("Could not open or parse sensors file.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//
//        SensorSelectionDialog dialog(this, sensors);
//        if (dialog.ShowModal() == wxID_OK) {
//            int id;
//            wxString paramName;
//            if (dialog.GetSelectedSensor(id, paramName)) {
//                selectedSensorId = id;
//                selectedSensorParamName = paramName;
//                UpdateSelectedSensorDisplay();
//            }
//        }
//
//        std::string sensorDirPath = "pomiary/" + std::to_string(selectedStationId) + "/" + std::to_string(selectedSensorId);
//        std::filesystem::create_directories(sensorDirPath);
//
//        std::string apiUrl = "http://api.gios.gov.pl/pjp-api/rest/data/getData/" + std::to_string(selectedSensorId);
//        std::string readBuffer;
//
//        if (!PerformHttpGet(apiUrl, readBuffer)) {
//            wxMessageBox("Failed to download sensor data.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//
//        Json::Value root;
//        Json::CharReaderBuilder builder;
//        std::string errors;
//        std::istringstream jsonStream(readBuffer);
//
//        bool parsingSuccessful = Json::parseFromStream(builder, jsonStream, &root, &errors);
//        if (!parsingSuccessful || !root.isMember("values") || !root["values"].isArray()) {
//            wxMessageBox("Failed to parse sensor data.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//
//        if (!ProcessAndSaveSensorData(root, sensorDirPath)) {
//            wxMessageBox("Failed to process or save sensor data.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//    }
//
//
//    void OnShowDataFiles(wxCommandEvent& event) {
//        if (selectedSensorId == -1) {
//            wxMessageBox("Please select a sensor first.", "No Sensor Selected", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        std::string sensorDirPath = "pomiary/" + std::to_string(selectedStationId) + "/" + std::to_string(selectedSensorId);
//
//        if (!std::filesystem::exists(sensorDirPath) || !std::filesystem::is_directory(sensorDirPath)) {
//            wxMessageBox("No data files found for this sensor. Please collect data first.",
//                "No Data", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        std::vector<std::string> dataFiles;
//        for (const auto& entry : std::filesystem::directory_iterator(sensorDirPath)) {
//            if (entry.is_regular_file() && entry.path().extension() == ".json") {
//                dataFiles.push_back(entry.path().filename().string());
//            }
//        }
//
//        if (dataFiles.empty()) {
//            wxMessageBox("No data files found for this sensor. Please collect data first.",
//                "No Data", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        FileSelectionDialog dialog(this, dataFiles);
//        if (dialog.ShowModal() == wxID_OK) {
//            wxString selectedFile = dialog.GetSelectedFile();
//            if (!selectedFile.IsEmpty()) {
//                selectedFileName = selectedFile;
//                UpdateSelectedFileDisplay();
//
//                ProcessFileData(sensorDirPath + "/" + selectedFile.ToStdString());
//            }
//        }
//    }
//
//
//    // Helper methods
//    bool LoadJsonFromFile(const std::string& filePath, Json::Value& output) {
//        std::ifstream file(filePath);
//        if (!file.is_open()) {
//            return false;
//        }
//
//        Json::CharReaderBuilder builder;
//        std::string errors;
//        bool parsingSuccessful = Json::parseFromStream(builder, file, &output, &errors);
//        file.close();
//
//        return parsingSuccessful;
//    }
//
//    bool ProcessAndSaveSensorData(const Json::Value& data, const std::string& dirPath) {
//        const Json::Value& values = data["values"];
//
//        Json::Value filteredValues(Json::arrayValue);
//        std::string firstValidDate;
//        std::string lastValidDate;
//
//        for (Json::Value::ArrayIndex i = 0; i < values.size(); i++) {
//            const Json::Value& item = values[i];
//            if (item.isMember("date") && item.isMember("value") && !item["value"].isNull()) {
//                filteredValues.append(item);
//
//                std::string currentDate = item["date"].asString();
//
//                if (firstValidDate.empty()) {
//                    firstValidDate = currentDate;
//                }
//
//                lastValidDate = currentDate;
//            }
//        }
//
//        if (filteredValues.size() == 0) {
//            wxMessageBox("No valid data points found for this sensor.", "Warning", wxOK | wxICON_WARNING);
//            return false;
//        }
//
//        std::string firstDateForFilename = FormatDateForFilename(firstValidDate);
//        std::string lastDateForFilename = FormatDateForFilename(lastValidDate);
//
//        std::string filename = lastDateForFilename + "_to_" + firstDateForFilename + ".json";
//        std::string filePath = dirPath + "/" + filename;
//
//        Json::Value result;
//        if (data.isMember("key")) {
//            result["key"] = data["key"];
//        }
//        result["values"] = filteredValues;
//
//        std::ofstream file(filePath);
//        if (!file.is_open()) {
//            return false;
//        }
//
//        Json::StreamWriterBuilder writerBuilder;
//        writerBuilder["indentation"] = "  ";
//        std::unique_ptr<Json::StreamWriter> writer(writerBuilder.newStreamWriter());
//        writer->write(result, &file);
//        file.close();
//
//        return true;
//    }
//
//    std::string FormatDateForFilename(const std::string& dateStr) {
//        if (dateStr.length() < 13) {
//            return "unknown";
//        }
//
//        std::string formatted = dateStr.substr(0, 10) + "_" + dateStr.substr(11, 2);
//        return formatted;
//    }
//
//    void ProcessFileData(const std::string& filePath) {
//        Json::Value data;
//        if (!LoadJsonFromFile(filePath, data)) {
//            wxMessageBox("Could not open or parse data file.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//
//        if (!data.isMember("values") || !data["values"].isArray()) {
//            wxMessageBox("Failed to parse data file: missing values array.", "Error", wxOK | wxICON_ERROR);
//            return;
//        }
//
//        std::string key = data.isMember("key") ? data["key"].asString() : "Unknown";
//        std::string paramCode = data.isMember("param") ? data["param"].asString() : "";
//
//        const Json::Value& values = data["values"];
//        if (values.size() == 0) {
//            wxMessageBox("No values found in the data file.", "Warning", wxOK | wxICON_WARNING);
//            return;
//        }
//
//        double max = std::numeric_limits<double>::lowest();
//        double min = std::numeric_limits<double>::max();
//        double sum = 0.0;
//        int count = 0;
//        std::vector<std::pair<std::string, double>> timeSeriesData;
//        std::vector<std::pair<wxString, double>> chartData;
//
//        for (Json::Value::ArrayIndex i = 0; i < values.size(); i++) {
//            const Json::Value& item = values[i];
//            if (item.isMember("value") && !item["value"].isNull() && item.isMember("date")) {
//                double value = item["value"].asDouble();
//                std::string dateStr = item["date"].asString();
//
//                max = std::max(max, value);
//                min = std::min(min, value);
//                sum += value;
//                count++;
//
//                timeSeriesData.push_back(std::make_pair(dateStr, value));
//                chartData.push_back(std::make_pair(wxString(dateStr), value));
//            }
//        }
//
//        std::sort(chartData.begin(), chartData.end(),
//            [](const std::pair<wxString, double>& a, const std::pair<wxString, double>& b) {
//                return a.first < b.first;
//            });
//
//        double average = (count > 0) ? (sum / count) : 0.0;
//
//        std::string trend = CalculateTrend(timeSeriesData);
//
//        if (infoPanel) {
//            infoPanel->UpdateStatistics(key, max, min, average, trend);
//        }
//
//        wxString chartTitle = wxString::Format("Air Quality Measurements: %s", key);
//        wxString yAxisLabel = paramCode;
//
//        if (yAxisLabel.IsEmpty() && !key.empty()) {
//            size_t pos = key.find('(');
//            if (pos != std::string::npos) {
//                size_t end = key.find(')', pos);
//                if (end != std::string::npos) {
//                    yAxisLabel = key.substr(pos + 1, end - pos - 1);
//                }
//            }
//
//            if (yAxisLabel.IsEmpty()) {
//                yAxisLabel = "Value";
//            }
//        }
//
//        if (chartPanel && !chartData.empty()) {
//            chartPanel->SetChartData(chartData, chartTitle, "Date", yAxisLabel);
//        }
//    }
//
//    std::string CalculateTrend(const std::vector<std::pair<std::string, double>>& data) {
//        if (data.size() < 2) {
//            return "insufficient data for trend analysis";
//        }
//
//        std::vector<std::pair<std::string, double>> sortedData = data;
//        std::sort(sortedData.begin(), sortedData.end(),
//            [](const std::pair<std::string, double>& a, const std::pair<std::string, double>& b) {
//                return a.first < b.first;
//            });
//
//        int n = sortedData.size();
//        int firstThirdEnd = n / 3;
//        int lastThirdStart = 2 * n / 3;
//
//        double firstThirdSum = 0.0;
//        for (int i = 0; i < firstThirdEnd; i++) {
//            firstThirdSum += sortedData[i].second;
//        }
//        double firstThirdAvg = firstThirdSum / firstThirdEnd;
//
//        double lastThirdSum = 0.0;
//        for (int i = lastThirdStart; i < n; i++) {
//            lastThirdSum += sortedData[i].second;
//        }
//        double lastThirdAvg = lastThirdSum / (n - lastThirdStart);
//
//        double difference = lastThirdAvg - firstThirdAvg;
//        double percentChange = (firstThirdAvg != 0) ? (difference / firstThirdAvg) * 100.0 : 0.0;
//
//        if (std::abs(percentChange) < 5.0) {
//            return "stable";
//        }
//        else if (percentChange > 0) {
//            if (percentChange > 20.0) {
//                return "strongly increasing";
//            }
//            else {
//                return "increasing";
//            }
//        }
//        else {
//            if (percentChange < -20.0) {
//                return "strongly decreasing";
//            }
//            else {
//                return "decreasing";
//            }
//        }
//    }
//
//    // Add the event handler function:
//    void OnViewDownloadedData(wxCommandEvent& event) {
//        // Check if data directory exists
//        if (!std::filesystem::exists("pomiary")) {
//            wxMessageBox("No downloaded data found. Please download data first.",
//                "No Data", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        // Get all station directories
//        std::vector<std::pair<int, std::string>> stationDirs;
//        for (const auto& entry : std::filesystem::directory_iterator("pomiary")) {
//            if (entry.is_directory()) {
//                std::string dirName = entry.path().filename().string();
//                // Skip non-numeric directories (like stacje.json)
//                if (std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
//                    stationDirs.push_back({ std::stoi(dirName), dirName });
//                }
//            }
//        }
//
//        if (stationDirs.empty()) {
//            wxMessageBox("No station data found. Please download station data first.",
//                "No Data", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        // Create a selection dialog for stations
//        wxArrayString choices;
//        std::map<int, int> indexToStationId;
//        int idx = 0;
//
//        // Load station names from stations.json
//        Json::Value stations;
//        std::map<int, wxString> stationNames;
//
//        if (LoadJsonFromFile(STATIONS_FILE_PATH, stations) && stations.isArray()) {
//            for (const auto& station : stations) {
//                if (station.isMember("id") && station.isMember("stationName")) {
//                    int id = station["id"].asInt();
//                    wxString name = wxString::FromUTF8(station["stationName"].asString());
//                    stationNames[id] = name;
//                }
//            }
//        }
//
//        // Create display strings with names if available
//        for (const auto& [id, dirName] : stationDirs) {
//            wxString displayName;
//            if (stationNames.find(id) != stationNames.end()) {
//                displayName = wxString::Format("%d - %s", id, stationNames[id]);
//            }
//            else {
//                displayName = wxString::Format("%d", id);
//            }
//            choices.Add(displayName);
//            indexToStationId[idx++] = id;
//        }
//
//        wxSingleChoiceDialog stationDialog(this, "Select a station:", "Station Selection", choices);
//        if (stationDialog.ShowModal() != wxID_OK) {
//            return;
//        }
//
//        int selectedIndex = stationDialog.GetSelection();
//        selectedStationId = indexToStationId[selectedIndex];
//        selectedStationName = stationNames[selectedStationId];
//
//        UpdateSelectedStationDisplay();
//
//        // Now get sensor directories in the selected station
//        std::string stationPath = "pomiary/" + std::to_string(selectedStationId);
//        std::vector<int> sensorIds;
//
//        for (const auto& entry : std::filesystem::directory_iterator(stationPath)) {
//            if (entry.is_directory()) {
//                std::string dirName = entry.path().filename().string();
//                if (std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
//                    sensorIds.push_back(std::stoi(dirName));
//                }
//            }
//        }
//
//        if (sensorIds.empty()) {
//            wxMessageBox("No sensor data found for this station. Please download sensor data first.",
//                "No Data", wxOK | wxICON_INFORMATION);
//            return;
//        }
//
//        // Get sensor names from sensors.json
//        std::map<int, wxString> sensorNames;
//        Json::Value sensors;
//        std::string sensorsFilePath = stationPath + "/sensors.json";
//
//        if (std::filesystem::exists(sensorsFilePath) && LoadJsonFromFile(sensorsFilePath, sensors) && sensors.isArray()) {
//            for (const auto& sensor : sensors) {
//                if (sensor.isMember("id") && sensor.isMember("param") &&
//                    sensor["param"].isMember("paramName")) {
//                    int sensorId = sensor["id"].asInt();
//                    wxString paramName = wxString::FromUTF8(sensor["param"]["paramName"].asString());
//                    sensorNames[sensorId] = paramName;
//                }
//            }
//        }
//
//        // Create choices with names if available
//        wxArrayString sensorChoices;
//        std::map<int, int> indexToSensorId;
//        idx = 0;
//
//        for (const int& sensorId : sensorIds) {
//            wxString displayName;
//            if (sensorNames.find(sensorId) != sensorNames.end()) {
//                displayName = wxString::Format("%d - %s", sensorId, sensorNames[sensorId]);
//            }
//            else {
//                displayName = wxString::Format("%d", sensorId);
//            }
//            sensorChoices.Add(displayName);
//            indexToSensorId[idx++] = sensorId;
//        }
//
//        wxSingleChoiceDialog sensorDialog(this, "Select a sensor:", "Sensor Selection", sensorChoices);
//        if (sensorDialog.ShowModal() != wxID_OK) {
//            return;
//        }
//
//        selectedIndex = sensorDialog.GetSelection();
//        selectedSensorId = indexToSensorId[selectedIndex];
//
//        if (sensorNames.find(selectedSensorId) != sensorNames.end()) {
//            selectedSensorParamName = sensorNames[selectedSensorId];
//        }
//        else {
//            selectedSensorParamName = wxString::Format("Sensor %d", selectedSensorId);
//        }
//
//        UpdateSelectedSensorDisplay();
//
//        // Now show available data files for the selected sensor
//        OnShowDataFiles(event);
//    }
//
//
//};


class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Air quality APP", wxDefaultPosition, wxSize(1400, 800)) {
        SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        SidePanel* sidePanel = new SidePanel(this);

        wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
        ChartPanel* chartPanel = new ChartPanel(this);
        InfoPanel* infoPanel = new InfoPanel(this);

        sidePanel->ConnectToPanels(infoPanel, chartPanel);

        rightSizer->Add(chartPanel, 0, wxEXPAND | wxALL, 5);
        rightSizer->Add(infoPanel, 1, wxEXPAND | wxALL, 5);

        mainSizer->Add(sidePanel, 0, wxEXPAND);
        mainSizer->Add(rightSizer, 1, wxEXPAND);

        SetSizer(mainSizer);
        Centre();
    }
};

class MyApp : public wxApp {
private:
    wxLocale* m_locale = nullptr;

public:
    bool OnInit() override {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        wxFont::AddPrivateFont("C:\\Windows\\Fonts\\arial.ttf");

        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }

    int OnExit() override {
        delete m_locale;
        curl_global_cleanup();
        return wxApp::OnExit();
    }
};

wxIMPLEMENT_APP(MyApp);