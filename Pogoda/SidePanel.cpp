#include "SidePanel.h"

//Constructor and mainwindow connection
SidePanel::SidePanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(200, 800)) {

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

void SidePanel::ConnectToPanels(InfoPanel* info, ChartPanel* chart) {
    infoPanel = info;
    chartPanel = chart;
}

//UI inicialization
void SidePanel::InitializeUI() {
    wxBoxSizer* btnSizer = new wxBoxSizer(wxVERTICAL);

    btn1 = new wxButton(this, wxID_ANY, "Chose station");
    btn2 = new wxButton(this, wxID_ANY, "Chose sensor");
    btn3 = new wxButton(this, wxID_ANY, "Select data");
    btn4 = new wxButton(this, wxID_ANY, "View downloaded data");

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

//Binding
void SidePanel::BindEvents() {
    btn1->Bind(wxEVT_BUTTON, &SidePanel::OnChoseStation, this);
    btn2->Bind(wxEVT_BUTTON, &SidePanel::OnChooseSensor, this);
    btn3->Bind(wxEVT_BUTTON, &SidePanel::OnShowDataFiles, this);
    btn4->Bind(wxEVT_BUTTON, &SidePanel::OnViewDownloadedData, this);
}

//Status display updates
void SidePanel::UpdateSelectedStationDisplay() {
    if (selectedStationId != -1) {
        wxString info = wxString::Format("Selected station:\n%s (ID: %d)",
            selectedStationName, selectedStationId);
        selectedStationText->SetLabel(info);
    }
    else {
        selectedStationText->SetLabel("No station selected");
    }
}

void SidePanel::UpdateSelectedSensorDisplay() {
    if (selectedSensorId != -1) {
        wxString info = wxString::Format("Selected sensor:\n%s (ID: %d)",
            selectedSensorParamName, selectedSensorId);
        selectedSensorText->SetLabel(info);
    }
    else {
        selectedSensorText->SetLabel("No sensor selected");
    }
}

void SidePanel::UpdateSelectedFileDisplay() {
    if (!selectedFileName.IsEmpty()) {
        wxString info = wxString::Format("Selected file:\n%s", selectedFileName);
        selectedFileText->SetLabel(info);
    }
    else {
        selectedFileText->SetLabel("No file selected");
    }
}

//Button binded functions
void SidePanel::OnChoseStation(wxCommandEvent& event) {
    std::string DATABASE_STATIONS = DATABASE_DIRECTORRY + "/stations.json";
    std::filesystem::create_directories(DATABASE_DIRECTORRY);

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

void SidePanel::OnChooseSensor(wxCommandEvent& event) {
    if (selectedStationId == -1) {
        wxMessageBox("Please select a station first.", "No Station Selected", wxOK | wxICON_INFORMATION);
        return;
    }

    std::string STATION_DIRECTORY = DATABASE_DIRECTORRY + "/" + std::to_string(selectedStationId);
    std::string DATABASE_SENSORS = STATION_DIRECTORY + "/sensors.json";
    std::string apiUrl = SENSORS_API_URL + std::to_string(selectedStationId);
    std::filesystem::create_directories(STATION_DIRECTORY);

    if (!std::filesystem::exists(DATABASE_SENSORS)) {
        if (!DownloadJsonAndSaveToFile(apiUrl, DATABASE_SENSORS)) {
            return;
        }
    }
    Json::Value sensors;
    if (!LoadJsonFromFile(DATABASE_SENSORS, sensors)) {
        wxMessageBox("Could not open or parse sensors file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Start downloading data for all sensors in parallel
    std::vector<std::thread> downloadThreads;
    std::mutex downloadMutex;
    std::vector<std::string> failedSensors;

    // Function to download data for a single sensor
    auto downloadSensorData = [&](int sensorId) {
        std::string SENSORS_DIRECTORY = STATION_DIRECTORY + "/" + std::to_string(sensorId);
        std::filesystem::create_directories(SENSORS_DIRECTORY);
        std::string sensorApiUrl = SENSOR_API_URL + std::to_string(sensorId);

        bool success = DownloadJsonAndSaveData(sensorApiUrl, SENSORS_DIRECTORY);

        if (!success) {
            std::lock_guard<std::mutex> lock(downloadMutex);
            failedSensors.push_back(std::to_string(sensorId));
        }
    };

    // Create and start threads for each sensor
    for (auto const& sensor : sensors) {
        if (sensor.isMember("id") && sensor["id"].isInt()) {
            int sensorId = sensor["id"].asInt();
            downloadThreads.push_back(std::thread(downloadSensorData, sensorId));
        }
    }

    // Show a dialog to let the user select a sensor while downloads are in progress
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

    // Wait for all downloads to complete
    for (auto& thread : downloadThreads) {
        if (thread.joinable()) {
            thread.join();
        }
    }

    // Report any download failures
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

    // Handle the selected sensor (which is now already downloaded along with all others)
    if (selectedSensorId != -1) {
        std::string SENSORS_DIRECTORY = STATION_DIRECTORY + "/" + std::to_string(selectedSensorId);
        // Check if the selected sensor's data was downloaded successfully
        if (std::find(failedSensors.begin(), failedSensors.end(), std::to_string(selectedSensorId)) != failedSensors.end()) {
            // If the selected sensor's download failed, try one more time directly
            std::string apiUrl = SENSOR_API_URL + std::to_string(selectedSensorId);
            if (!DownloadJsonAndSaveData(apiUrl, SENSORS_DIRECTORY)) {
                wxMessageBox("Failed to download data for the selected sensor.", "Error", wxOK | wxICON_ERROR);
            }
        }
    }
}

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

void SidePanel::OnViewDownloadedData(wxCommandEvent& event) {
    // Check if data directory exists
    if (!std::filesystem::exists(DATABASE_DIRECTORRY)) {
        wxMessageBox("No downloaded data found. Please download data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return;
    }

    // Get all station directories
    std::vector<int> stationDirIds;
    for (const auto& entry : std::filesystem::directory_iterator(DATABASE_DIRECTORRY)) {
        if (entry.is_directory()) {
            std::string dirName = entry.path().filename().string();
            // Skip non-numeric directories (like stations.json)
            if (std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                stationDirIds.push_back(std::stoi(dirName));
            }
        }
    }

    if (stationDirIds.empty()) {
        wxMessageBox("No station data found. Please download station data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return;
    }

    // Load the full stations data from stations.json
    Json::Value allStations;
    if (!LoadJsonFromFile(DATABASE_STATIONS, allStations)) {
        wxMessageBox("Could not open or parse stations file.", "Error", wxOK | wxICON_ERROR);
        return;
    }

    // Create a filtered list of stations that have directories
    Json::Value filteredStations(Json::arrayValue);
    for (const auto& station : allStations) {
        if (station.isMember("id")) {
            int stationId = station["id"].asInt();
            // Check if this station has a directory
            if (std::find(stationDirIds.begin(), stationDirIds.end(), stationId) != stationDirIds.end()) {
                filteredStations.append(station);
            }
        }
    }

    // Use the StationSelectionDialog with filtered stations
    StationSelectionDialog stationDialog(this, filteredStations);
    if (stationDialog.ShowModal() == wxID_OK) {
        int id;
        wxString name;
        if (stationDialog.GetSelectedStation(id, name)) {
            selectedStationId = id;
            selectedStationName = name;
            UpdateSelectedStationDisplay();

            selectedSensorId = -1;
            selectedSensorParamName = "";
            UpdateSelectedSensorDisplay();

            // Find sensor directories in the selected station
            std::string STATION_DIRECTORY = DATABASE_DIRECTORRY + "/" + std::to_string(selectedStationId);
            std::vector<int> sensorDirIds;

            for (const auto& entry : std::filesystem::directory_iterator(STATION_DIRECTORY)) {
                if (entry.is_directory()) {
                    std::string dirName = entry.path().filename().string();
                    if (std::all_of(dirName.begin(), dirName.end(), ::isdigit)) {
                        sensorDirIds.push_back(std::stoi(dirName));
                    }
                }
            }

            if (sensorDirIds.empty()) {
                wxMessageBox("No sensor data found for this station. Please download sensor data first.",
                    "No Data", wxOK | wxICON_INFORMATION);
                return;
            }

            // Load sensors data
            std::string DATABASE_SENSORS = STATION_DIRECTORY + "/sensors.json";
            Json::Value allSensors;
            if (!std::filesystem::exists(DATABASE_SENSORS) || !LoadJsonFromFile(DATABASE_SENSORS, allSensors)) {
                wxMessageBox("Could not open or parse sensors file.", "Error", wxOK | wxICON_ERROR);
                return;
            }

            // Create a filtered list of sensors that have directories
            Json::Value filteredSensors(Json::arrayValue);
            for (const auto& sensor : allSensors) {
                if (sensor.isMember("id")) {
                    int sensorId = sensor["id"].asInt();
                    // Check if this sensor has a directory
                    if (std::find(sensorDirIds.begin(), sensorDirIds.end(), sensorId) != sensorDirIds.end()) {
                        filteredSensors.append(sensor);
                    }
                }
            }

            // Use the SensorSelectionDialog with filtered sensors
            SensorSelectionDialog sensorDialog(this, filteredSensors);
            if (sensorDialog.ShowModal() == wxID_OK) {
                int sensorId;
                wxString paramName;
                if (sensorDialog.GetSelectedSensor(sensorId, paramName)) {
                    selectedSensorId = sensorId;
                    selectedSensorParamName = paramName;
                    UpdateSelectedSensorDisplay();

                    // Now show available data files for the selected sensor
                    OnShowDataFiles(event);
                }
            }
        }
    }
}