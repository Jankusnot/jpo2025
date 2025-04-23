/**
 * @file SidePanel.cpp
 * @brief Implementation of the SidePanel class
 */


#include "SidePanel.h"

/**
 * @brief Constructor that initializes the side panel with default settings and UI elements
 * @param parent The parent window that will contain this panel
 *
 * Creates the panel with a fixed width of 350px and a height of 800px.
 * Sets a light gray background color and initializes all UI elements and selection state variables.
 */
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

/**
 * @brief Connects this side panel to other panels for data communication
 * @param info Pointer to the InfoPanel instance that displays textual information
 * @param chart Pointer to the ChartPanel instance that displays graphical data
 *
 * This method must be called after panel creation to establish communication channels
 * between this panel and the information/chart display panels.
 */
void SidePanel::ConnectToPanels(InfoPanel* info, ChartPanel* chart) {
    infoPanel = info;
    chartPanel = chart;
}

/**
 * @brief Creates and arranges all UI elements including buttons and status text displays
 *
 * Sets up the vertical layout with buttons for station selection, sensor selection,
 * data file selection, and data viewing. Also creates status text labels to display
 * the currently selected items.
 */
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

/**
 * @brief Connects button events to their handler functions
 *
 * Binds each button's click event to the appropriate handler method
 * to respond when the user interacts with the UI.
 */
void SidePanel::BindEvents() {
    btn1->Bind(wxEVT_BUTTON, &SidePanel::OnChoseStation, this);
    btn2->Bind(wxEVT_BUTTON, &SidePanel::OnChooseSensor, this);
    btn3->Bind(wxEVT_BUTTON, &SidePanel::OnShowDataFiles, this);
    btn4->Bind(wxEVT_BUTTON, &SidePanel::OnViewStoredData, this);
}

/**
 * @brief Updates the display text showing the currently selected station
 *
 * Changes the label of the station status text to show the ID and name
 * of the currently selected station, or "No station selected" if none is selected.
 */
void SidePanel::UpdateSelectedStationDisplay() {
    if (selectedStationId != -1) {
        wxString info = wxString::Format("Selected station:\n(ID: %d) %s", selectedStationId, selectedStationName);
        selectedStationText->SetLabel(info);
    }
    else {
        selectedStationText->SetLabel("No station selected");
    }
}

/**
 * @brief Updates the display text showing the currently selected sensor
 *
 * Changes the label of the sensor status text to show the ID and parameter name
 * of the currently selected sensor, or "No sensor selected" if none is selected.
 */
void SidePanel::UpdateSelectedSensorDisplay() {
    if (selectedSensorId != -1) {
        wxString info = wxString::Format("Selected sensor:\n(ID: %d) %s", selectedSensorId, selectedSensorParamName);
        selectedSensorText->SetLabel(info);
    }
    else {
        selectedSensorText->SetLabel("No sensor selected");
    }
}

/**
 * @brief Updates the display text showing the currently selected data file
 *
 * Changes the label of the file status text to show the name of the
 * currently selected data file, or "No file selected" if none is selected.
 */
void SidePanel::UpdateSelectedFileDisplay() {
    if (!selectedFileName.IsEmpty()) {
        wxString info = wxString::Format("Selected file:\n%s", selectedFileName);
        selectedFileText->SetLabel(info);
    }
    else {
        selectedFileText->SetLabel("No file selected");
    }
}

/**
 * @brief Handles station selection by downloading station data if needed and showing selection dialog
 * @param event The button click event (unused)
 *
 * This method:
 * 1. Ensures the data directory exists
 * 2. Downloads the station list from the API if it doesn't exist locally
 * 3. Opens a dialog for the user to select a station
 * 4. Updates the selection state and display when a station is selected
 * 5. Resets the sensor selection since it's no longer valid
 */
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

/**
 * @brief Handles sensor selection by downloading sensor data and showing selection dialog
 * @param event The button click event (unused)
 *
 * This method:
 * 1. Checks if a station is selected
 * 2. Downloads the sensor list for the station if it doesn't exist locally
 * 3. Starts parallel downloads for all sensor data
 * 4. Opens a dialog for the user to select a sensor
 * 5. Updates the selection state and display when a sensor is selected
 * 6. Reports any download failures and ensures the selected sensor's data is available
 */
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

/**
 * @brief Handles selecting data files for the currently selected sensor
 * @param event The button click event (unused)
 *
 * This method:
 * 1. Checks if a sensor is selected
 * 2. Verifies that data files exist for the selected sensor
 * 3. Opens a dialog for the user to select a data file
 * 4. Updates the selection state and display when a file is selected
 * 5. Processes the selected file and updates the info and chart panels
 */
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

/**
 * @brief Provides a workflow for viewing stored data by selecting station, sensor, and file
 * @param event The button click event (passed to OnShowDataFiles)
 *
 * This method creates a guided workflow for the user:
 * 1. Checks if any data exists
 * 2. Finds stations that have downloaded data
 * 3. Presents a station selection dialog
 * 4. After station selection, presents a sensor selection dialog
 * 5. After sensor selection, shows the data file selection dialog
 */
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

/////Helper functions for OnChoseSensor

/**
 * @brief Creates the sensor database directory and downloads sensor data if needed
 * @param STATION_DIRECTORY Directory path for the station data
 * @param databasePath Path to the sensor database file
 * @param apiUrl URL for the API endpoint to download sensor data
 * @return true if initialization succeeds, false otherwise
 *
 * Creates the directory structure for sensor data and downloads the sensors list
 * if it doesn't exist locally.
 */
bool SidePanel::InitializeSensorDatabase(const std::string& STATION_DIRECTORY, const std::string& databasePath, const std::string& apiUrl) {
    std::filesystem::create_directories(STATION_DIRECTORY);
    if (!std::filesystem::exists(databasePath)) {
        return DownloadJsonAndSaveToFile(apiUrl, databasePath);
    }
    return true;
}

/**
 * @brief Loads sensor data from JSON file into the provided object
 * @param databasePath Path to the sensor database file
 * @param sensors JSON value to load the data into
 * @return true if data was successfully loaded, false otherwise
 *
 * Attempts to load and parse the sensor database file, displaying an error
 * message if the operation fails.
 */
bool SidePanel::LoadSensorsData(const std::string& databasePath, Json::Value& sensors) {
    if (!LoadJsonFromFile(databasePath, sensors)) {
        wxMessageBox("Could not open or parse sensors file.", "Error", wxOK | wxICON_ERROR);
        return false;
    }
    return true;
}

/**
 * @brief Downloads data for a specific sensor and records any failures
 * @param sensorId ID of the sensor to download data for
 * @param STATION_DIRECTORY Directory path for the station data
 * @param failedSensors Vector to record failed sensor IDs
 * @param mutex Mutex for thread-safe access to the failedSensors vector
 * @return true if download succeeds, false otherwise
 *
 * Creates the directory for the sensor data and attempts to download the data.
 * If the download fails, the sensor ID is added to the failedSensors vector.
 */
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

/**
 * @brief Starts parallel downloads for all sensors in the station
 * @param STATION_DIRECTORY Directory path for the station data
 * @param sensors JSON value containing the sensors data
 * @param failedSensors Vector to record failed sensor IDs
 *
 * Creates and manages threads to download data for all sensors in parallel,
 * improving the download performance by utilizing multiple connections.
 */
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

/**
 * @brief Shows a message box listing any sensors that failed to download
 * @param failedSensors Vector containing IDs of sensors that failed to download
 *
 * If any sensors failed to download, displays a warning message with the list
 * of sensor IDs that could not be downloaded.
 */
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

/**
 * @brief Makes sure data for the selected sensor is available, retrying download if needed
 * @param STATION_DIRECTORY Directory path for the station data
 * @param failedSensors Vector containing IDs of sensors that failed to download
 *
 * If the selected sensor's data download failed, attempts to download it again
 * to ensure that the user can proceed with the selected sensor.
 */
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

/////Helper functions for OnViewStoredData

/**
 * @brief Verifies that downloaded data exists before trying to access it
 * @return true if data is available, false otherwise
 *
 * Checks if the data directory exists and displays a message if no data is found.
 */
bool SidePanel::CheckForDownloadedData() {
    if (!std::filesystem::exists(DATABASE_DIRECTORRY)) {
        wxMessageBox("No downloaded data found. Please download data first.",
            "No Data", wxOK | wxICON_INFORMATION);
        return false;
    }
    return true;
}

/**
 * @brief Gets a list of numeric directories in the specified path
 * @param directory Path to search for numeric directories
 * @return Vector of directory IDs as integers
 *
 * Scans the specified directory for subdirectories with numeric names,
 * which represent station or sensor IDs in the application's file structure.
 */
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

/**
 * @brief Filters a list of items to only include those with existing directories
 * @param allItems JSON value containing all items
 * @param dirIds Vector of directory IDs
 * @return Filtered JSON value containing only items with existing directories
 *
 * Creates a filtered list of items (stations or sensors) that have corresponding
 * directories in the file system, ensuring that only items with data are shown.
 */
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

/**
 * @brief Shows a dialog for selecting a station and updates the selection state
 * @param filteredStations JSON value containing filtered station data
 * @return true if a station was selected, false otherwise
 *
 * Opens a dialog showing available stations with data and updates the
 * selection state if the user selects a station.
 */
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

/**
 * @brief Shows a dialog for selecting a sensor from the filtered list and updates the selection state
 * @return true if a sensor was selected, false otherwise
 *
 * Finds sensors with data for the selected station, filters the list,
 * and shows a dialog for the user to select a sensor.
 */
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