#include "SidePanel.h"

// Constructor that initializes the side panel with default settings and UI elements
SidePanel::SidePanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(350, 800)) {

    SetBackgroundColour(wxColour(220, 220, 220));

    InitializeUI();
    BindEvents();

    // Initialize selection state
    selectedStationId = -1;
    selectedStationName = "";
    selectedSensorId = -1;
    selectedSensorParamName = "";
    selectedFileName = "";
}

// Connects this side panel to other panels for data communication
void SidePanel::ConnectToPanels(InfoPanel* info, ChartPanel* chart) {
    infoPanel = info;
    chartPanel = chart;
}

// Creates and arranges all UI elements including buttons and status text displays
void SidePanel::InitializeUI() {
    wxBoxSizer* btnSizer = new wxBoxSizer(wxVERTICAL);

    btn1 = new wxButton(this, wxID_ANY, "Chose station");
    btn2 = new wxButton(this, wxID_ANY, "Chose sensor");
    btn3 = new wxButton(this, wxID_ANY, "Select data");
    btn4 = new wxButton(this, wxID_ANY, "View stored data");

    // Set button fonts
    wxFont buttonFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD);

    btn1->SetFont(buttonFont);
    btn2->SetFont(buttonFont);
    btn3->SetFont(buttonFont);
    btn4->SetFont(buttonFont);

    // Initialize status text controls
    wxFont statusFont(9, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL);

    selectedStationText = new wxStaticText(this, wxID_ANY, "No station selected");
    selectedStationText->SetFont(statusFont);
    selectedSensorText = new wxStaticText(this, wxID_ANY, "No sensor selected");
    selectedSensorText->SetFont(statusFont);
    selectedFileText = new wxStaticText(this, wxID_ANY, "No file selected");
    selectedFileText->SetFont(statusFont);

    // Add components to sizer
    btnSizer->Add(btn1, 0, wxALL | wxEXPAND, 10);
    btnSizer->Add(btn2, 0, wxALL | wxEXPAND, 10);
    btnSizer->Add(btn3, 0, wxALL | wxEXPAND, 10);
    btnSizer->Add(btn4, 0, wxALL | wxEXPAND, 10);
    btnSizer->Add(selectedStationText, 0, wxALL | wxEXPAND, 10);
    btnSizer->Add(selectedSensorText, 0, wxALL | wxEXPAND, 10);
    btnSizer->Add(selectedFileText, 0, wxALL | wxEXPAND, 10);

    SetSizer(btnSizer);
}

// Connects button events to their handler functions
void SidePanel::BindEvents() {
    btn1->Bind(wxEVT_BUTTON, &SidePanel::OnChoseStation, this);
    btn2->Bind(wxEVT_BUTTON, &SidePanel::OnChooseSensor, this);
    btn3->Bind(wxEVT_BUTTON, &SidePanel::OnShowDataFiles, this);
    btn4->Bind(wxEVT_BUTTON, &SidePanel::OnViewStoredData, this);
}

// Updates the display text showing the currently selected station
void SidePanel::UpdateSelectedStationDisplay() {
    if (selectedStationId != -1) {
        wxString info = wxString::Format("Selected station:\n(ID: %d) %s", selectedStationId, selectedStationName);
        selectedStationText->SetLabel(info);
    }
    else {
        selectedStationText->SetLabel("No station selected");
    }
}

// Updates the display text showing the currently selected sensor
void SidePanel::UpdateSelectedSensorDisplay() {
    if (selectedSensorId != -1) {
        wxString info = wxString::Format("Selected sensor:\n(ID: %d) %s", selectedSensorId, selectedSensorParamName);
        selectedSensorText->SetLabel(info);
    }
    else {
        selectedSensorText->SetLabel("No sensor selected");
    }
}

// Updates the display text showing the currently selected data file
void SidePanel::UpdateSelectedFileDisplay() {
    if (!selectedFileName.IsEmpty()) {
        wxString info = wxString::Format("Selected file:\n%s", selectedFileName);
        selectedFileText->SetLabel(info);
    }
    else {
        selectedFileText->SetLabel("No file selected");
    }
}

// Handles station selection by downloading station data if needed and showing selection dialog
void SidePanel::OnChoseStation(wxCommandEvent& event) {
    std::string DATABASE_STATIONS = DATABASE_DIRECTORRY + "/stations.json";
    std::filesystem::create_directories(DATABASE_DIRECTORRY);

    // Handles download of the station list
    if (!std::filesystem::exists(DATABASE_STATIONS)) {
        auto stationTransformer = [](const Json::Value& root) {
            Json::Value filteredStations(Json::arrayValue);

            for (Json::Value::ArrayIndex i = 0; i < root.size(); i++) {
                const Json::Value& station = root[i];
                Json::Value filteredStation;

                if (station.isMember("id"))
                    filteredStation["id"] = station["id"];

                if (station.isMember("stationName"))
                    filteredStation["stationName"] = station["stationName"];

                if (station.isMember("gegrLat"))
                    filteredStation["gegrLat"] = station["gegrLat"];

                if (station.isMember("gegrLon"))
                    filteredStation["gegrLon"] = station["gegrLon"];

                filteredStations.append(filteredStation);
            }
            return filteredStations;
        };

        bool success = DownloadJsonAndSaveToFile(STATIONS_API_URL, DATABASE_STATIONS, stationTransformer);
        if (!success) {
            return;
        }
    }

    Json::Value stations;
    if (!LoadJsonFromFile(DATABASE_STATIONS, stations)) {
        wxMessageBox("Could not open or parse stations file.", "Error", wxOK | wxICON_ERROR);
        return;
    }
    
    // Invokes station selection class to chose station and updates display for chosen station
    StationSelectionDialog dialog(this, stations);
    if (dialog.ShowModal() == wxID_OK) {
        int id;
        wxString name;
        if (dialog.GetSelectedStation(id, name)) {
            selectedStationId = id;
            selectedStationName = name;
            UpdateSelectedStationDisplay();

            selectedSensorId = -1;
            selectedSensorParamName = "";
            UpdateSelectedSensorDisplay();
        }
    }
}

// Handles sensor selection by downloading sensor data and showing selection dialog
void SidePanel::OnChooseSensor(wxCommandEvent& event) {
    if (selectedStationId == -1) {
        wxMessageBox("Please select a station first.", "No Station Selected", wxOK | wxICON_INFORMATION);
        return;
    }

    std::string STATION_DIRECTORY = DATABASE_DIRECTORRY + "/" + std::to_string(selectedStationId);
    std::string DATABASE_SENSORS = STATION_DIRECTORY + "/sensors.json";
    std::string apiUrl = SENSORS_API_URL + std::to_string(selectedStationId);

    // Downloads list of sensors for station
    if (!InitializeSensorDatabase(STATION_DIRECTORY, DATABASE_SENSORS, apiUrl)) {
        return;
    }

    // Loads list of sensor into json object
    Json::Value sensors;
    if (!LoadSensorsData(DATABASE_SENSORS, sensors)) {
        return;
    }

    // Start downloading data for all sensors in parallel
    std::vector<std::string> failedSensors;
    DownloadAllSensorsData(STATION_DIRECTORY, sensors, failedSensors);

    // Invokes sensor selection class to chose sensor and updates display for chosen sensor
    SensorSelectionDialog dialog(this, sensors);
    if (dialog.ShowModal() == wxID_OK) {
        int id;
        wxString paramName;
        if (dialog.GetSelectedSensor(id, paramName)) {
            selectedSensorId = id;
            selectedSensorParamName = paramName;
            UpdateSelectedSensorDisplay();
        }
    }

    // Report any download failures
    ReportFailedDownloads(failedSensors);

    // Ensure the selected sensor's data is available
    EnsureSelectedSensorData(STATION_DIRECTORY, failedSensors);
}

// Handles selecting data files for the currently selected sensor
void SidePanel::OnShowDataFiles(wxCommandEvent& event) {
    if (selectedSensorId == -1) {
        wxMessageBox("Please select a sensor first.", "No Sensor Selected", wxOK | wxICON_INFORMATION);
        return;
    }

    std::string SENSOR_DIRECTORY = DATABASE_DIRECTORRY + "/" + std::to_string(selectedStationId) + "/" + std::to_string(selectedSensorId);

    if (!std::filesystem::exists(SENSOR_DIRECTORY)) {
        wxMessageBox("No data files found for this sensor. Please collect data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return;
    }

    // Scans sensors directory for data files
    std::vector<std::string> dataFiles;
    for (const auto& entry : std::filesystem::directory_iterator(SENSOR_DIRECTORY)) {
        if (entry.is_regular_file() && entry.path().extension() == ".json") {
            dataFiles.push_back(entry.path().filename().string());
        }
    }

    if (dataFiles.empty()) {
        wxMessageBox("No data files found for this sensor. Please collect data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return;
    }

    // Invokes file selection class to chose data and updates display for chosen data entry
    FileSelectionDialog dialog(this, dataFiles);
    if (dialog.ShowModal() == wxID_OK) {
        wxString selectedFile = dialog.GetSelectedFile();
        if (!selectedFile.IsEmpty()) {
            selectedFileName = selectedFile;
            UpdateSelectedFileDisplay();

            ProcessFileData(SENSOR_DIRECTORY + "/" + selectedFile.ToStdString(), infoPanel, chartPanel);
        }
    }
}

void SidePanel::OnViewStoredData(wxCommandEvent& event) {
    // Check if data directory exists
    if (!CheckForDownloadedData()) {
        return;
    }

    // Get all station directories
    std::vector<int> stationDirIds = GetNumericDirectories(DATABASE_DIRECTORRY);

    if (stationDirIds.empty()) {
        wxMessageBox("No station data found. Please download station data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return;
    }

    // Load the full stations data
    Json::Value allStations;
    if (!LoadJsonFromFile(DATABASE_STATIONS, allStations)) {
        wxMessageBox("Could not open or parse stations file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Filter stations
    Json::Value filteredStations = FilterItemsByExistingDirectories(allStations, stationDirIds);

    // Select a station
    if (SelectStation(filteredStations)) {
        // Select a sensor
        if (SelectSensor()) {
            // Show available data files for the selected sensor
            OnShowDataFiles(event);
        }
    }
}

//////Helper functions for OnChoseSensor
// Creates the sensor database directory and downloads sensor data if needed
bool SidePanel::InitializeSensorDatabase(const std::string& STATION_DIRECTORY, const std::string& databasePath, const std::string& apiUrl) {
    std::filesystem::create_directories(STATION_DIRECTORY);
    if (!std::filesystem::exists(databasePath)) {
        return DownloadJsonAndSaveToFile(apiUrl, databasePath);
    }
    return true;
}

// Loads sensor data from JSON file into the provided object
bool SidePanel::LoadSensorsData(const std::string& databasePath, Json::Value& sensors) {
    if (!LoadJsonFromFile(databasePath, sensors)) {
        wxMessageBox("Could not open or parse sensors file.", "Error", wxOK | wxICON_ERROR);
        return false;
    }
    return true;
}

// Downloads data for a specific sensor and records any failures
bool SidePanel::DownloadSensorData(int sensorId, const std::string& STATION_DIRECTORY, std::vector<std::string>& failedSensors, std::mutex& mutex) {
    std::string sensorDirectory = STATION_DIRECTORY + "/" + std::to_string(sensorId);
    std::filesystem::create_directories(sensorDirectory);
    std::string sensorApiUrl = SENSOR_API_URL + std::to_string(sensorId);

    bool success = DownloadJsonAndSaveData(sensorApiUrl, sensorDirectory);

    if (!success) {
        std::lock_guard<std::mutex> lock(mutex);
        failedSensors.push_back(std::to_string(sensorId));
        return false;
    }
    return true;
}

// Starts parallel downloads for all sensors in the station
void SidePanel::DownloadAllSensorsData(const std::string& STATION_DIRECTORY, const Json::Value& sensors, std::vector<std::string>& failedSensors) {
    std::vector<std::thread> downloadThreads;
    std::mutex downloadMutex;

    // Create and start threads for each sensor
    for (auto const& sensor : sensors) {
        if (sensor.isMember("id") && sensor["id"].isInt()) {
            int sensorId = sensor["id"].asInt();
            downloadThreads.push_back(std::thread(&SidePanel::DownloadSensorData, this,
                sensorId, STATION_DIRECTORY, std::ref(failedSensors), std::ref(downloadMutex)));
        }
    }

    // Wait for all downloads to complete
    for (auto& thread : downloadThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }
}

// Shows a message box listing any sensors that failed to download
void SidePanel::ReportFailedDownloads(const std::vector<std::string>& failedSensors) {
    if (!failedSensors.empty()) {
        std::string failedList = "Failed to download data for sensors: ";
        for (size_t i = 0; i < failedSensors.size(); i++) {
            failedList += failedSensors[i];
            if (i < failedSensors.size() - 1) {
                failedList += ", ";
            }
        }
        wxMessageBox(failedList, "Download Warnings", wxOK | wxICON_WARNING);
    }
}

// Makes sure data for the selected sensor is available, retrying download if needed
void SidePanel::EnsureSelectedSensorData(const std::string& STATION_DIRECTORY, const std::vector<std::string>& failedSensors) {
    if (selectedSensorId != -1) {
        std::string sensorDirectory = STATION_DIRECTORY + "/" + std::to_string(selectedSensorId);

        // Check if the selected sensor's data was downloaded successfully
        if (std::find(failedSensors.begin(), failedSensors.end(), std::to_string(selectedSensorId)) != failedSensors.end()) {
            // If the selected sensor's download failed, try one more time directly
            std::string apiUrl = SENSOR_API_URL + std::to_string(selectedSensorId);
            if (!DownloadJsonAndSaveData(apiUrl, sensorDirectory)) {
                wxMessageBox("Failed to download data for the selected sensor.", "Error", wxOK | wxICON_ERROR);
            }
        }
    }
}

//////Helper functions for OnViewStoredData
// Verifies that downloaded data exists before trying to access it
bool SidePanel::CheckForDownloadedData() {
    if (!std::filesystem::exists(DATABASE_DIRECTORRY)) {
        wxMessageBox("No downloaded data found. Please download data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return false;
    }
    return true;
}

// Gets a list of directory IDs that are numeric (station or sensor IDs)
std::vector<int> SidePanel::GetNumericDirectories(const std::string& directory) {
    std::vector<int> dirIds;
    for (const auto& entry : std::filesystem::directory_iterator(directory)) {
        if (entry.is_directory()) {
            std::string dirName = entry.path().filename().string();
            // Skip non-numeric directories
            if (std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                dirIds.push_back(std::stoi(dirName));
            }
        }
    }
    return dirIds;
}

// Filters a list of items to only include those with directories
Json::Value SidePanel::FilterItemsByExistingDirectories(const Json::Value& allItems, const std::vector<int>& dirIds) {
    Json::Value filteredItems(Json::arrayValue);
    for (const auto& item : allItems) {
        if (item.isMember("id")) {
            int itemId = item["id"].asInt();
            // Check if this item has a directory
            if (std::find(dirIds.begin(), dirIds.end(), itemId) != dirIds.end()) {
                filteredItems.append(item);
            }
        }
    }
    return filteredItems;
}

// Shows a dialog for selecting a station and updates the selection state
bool SidePanel::SelectStation(Json::Value& filteredStations) {
    StationSelectionDialog stationDialog(this, filteredStations);
    if (stationDialog.ShowModal() == wxID_OK) {
        int id;
        wxString name;
        if (stationDialog.GetSelectedStation(id, name)) {
            selectedStationId = id;
            selectedStationName = name;
            UpdateSelectedStationDisplay();

            // Reset sensor selection
            selectedSensorId = -1;
            selectedSensorParamName = "";
            UpdateSelectedSensorDisplay();
            return true;
        }
    }
    return false;
}

// Shows a dialog for selecting a sensor from the filtered list and updates the selection state
bool SidePanel::SelectSensor() {
    std::string stationDirectory = DATABASE_DIRECTORRY + "/" + std::to_string(selectedStationId);
    std::vector<int> sensorDirIds = GetNumericDirectories(stationDirectory);

    if (sensorDirIds.empty()) {
        wxMessageBox("No sensor data found for this station. Please download sensor data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return false;
    }

    // Load sensors data
    std::string databaseSensors = stationDirectory + "/sensors.json";
    Json::Value allSensors;
    if (!std::filesystem::exists(databaseSensors) || !LoadJsonFromFile(databaseSensors, allSensors)) {
        wxMessageBox("Could not open or parse sensors file.", "Error", wxOK | wxICON_ERROR);
        return false;
    }

    // Filter sensors
    Json::Value filteredSensors = FilterItemsByExistingDirectories(allSensors, sensorDirIds);

    // Show sensor selection dialog
    SensorSelectionDialog sensorDialog(this, filteredSensors);
    if (sensorDialog.ShowModal() == wxID_OK) {
        int sensorId;
        wxString paramName;
        if (sensorDialog.GetSelectedSensor(sensorId, paramName)) {
            selectedSensorId = sensorId;
            selectedSensorParamName = paramName;
            UpdateSelectedSensorDisplay();
            return true;
        }
    }
    return false;
}