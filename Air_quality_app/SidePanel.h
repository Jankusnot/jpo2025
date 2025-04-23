#ifndef SIDE_PANEL_H
#define SIDE_PANEL_H

#define _CRT_SECURE_NO_WARNINGS


#include <wx/wx.h>
#include <wx/font.h>
#include <wx/sizer.h>
#include <wx/msgdlg.h>
#include <wx/listbox.h>
#include <wx/stattext.h>

#include <thread>
#include <mutex>
#include <json/json.h>
#include <filesystem>

#include "StationSelectionDialog.h"
#include "SensorSelectionDialog.h"
#include "FileSelectionDialog.h"

#include "ChartPanel.h"
#include "InfoPanel.h"

#include "DataProcessing.h"
#include "NetworkingAndFileHandling.h"

/**
 * @file SidePanel.cpp
 * @brief Implementation of the SidePanel class
 */

/**
 * @class SidePanel
 * @brief Panel containing controls for station, sensor, and file selection
 *
 * This panel provides the user interface for selecting monitoring stations,
 * sensors, and data files. It also manages the downloading and processing
 * of sensor data through API calls.
 */
class SidePanel : public wxPanel {
private:
    /// @name UI Components
    ///@{
    wxButton* btn1;                  ///< Button for station selection
    wxButton* btn2;                  ///< Button for sensor selection
    wxButton* btn3;                  ///< Button for showing data files
    wxButton* btn4;                  ///< Button for viewing stored data
    wxStaticText* selectedStationText; ///< Display text for selected station
    wxStaticText* selectedSensorText;  ///< Display text for selected sensor
    wxStaticText* selectedFileText;    ///< Display text for selected file
    ///@}

    /// @name Selection state
    ///@{
    int selectedStationId;           ///< ID of the currently selected station
    wxString selectedStationName;    ///< Name of the currently selected station
    int selectedSensorId;            ///< ID of the currently selected sensor
    wxString selectedSensorParamName; ///< Parameter name of the currently selected sensor
    wxString selectedFileName;        ///< Name of the currently selected data file
    ///@}

    /// @name Panel references
    ///@{
    InfoPanel* infoPanel;            ///< Reference to the information panel
    ChartPanel* chartPanel;          ///< Reference to the chart panel
    ///@}

    /// @name Constants
    ///@{
    const std::string STATIONS_API_URL = "http://api.gios.gov.pl/pjp-api/rest/station/findAll";     ///< API endpoint for all stations
    const std::string SENSORS_API_URL = "http://api.gios.gov.pl/pjp-api/rest/station/sensors/";     ///< API endpoint prefix for station sensors
    const std::string SENSOR_API_URL = "http://api.gios.gov.pl/pjp-api/rest/data/getData/";         ///< API endpoint prefix for sensor data
    const std::string DATABASE_DIRECTORRY = "measurements";                                          ///< Base directory for measurements data
    const std::string DATABASE_STATIONS = DATABASE_DIRECTORRY + "/stations.json";                    ///< Path to the stations database file
    ///@}

public:
    /**
     * @brief Constructor for the SidePanel
     * @param parent The parent window
     */
    SidePanel(wxWindow* parent);

    /**
     * @brief Connects this panel to the info and chart panels
     * @param info Pointer to the InfoPanel instance
     * @param chart Pointer to the ChartPanel instance
     */
    void ConnectToPanels(InfoPanel* info, ChartPanel* chart);

private:
    /// @name UI initialization methods
    ///@{
    /**
     * @brief Initializes all UI components
     */
    void InitializeUI();

    /**
     * @brief Binds event handlers to UI components
     */
    void BindEvents();
    ///@}

    /// @name Status display update methods
    ///@{
    /**
     * @brief Updates the selected station display text
     */
    void UpdateSelectedStationDisplay();

    /**
     * @brief Updates the selected sensor display text
     */
    void UpdateSelectedSensorDisplay();

    /**
     * @brief Updates the selected file display text
     */
    void UpdateSelectedFileDisplay();
    ///@}

    /// @name Button event handlers
    ///@{
    /**
     * @brief Handles the "Choose Station" button click
     * @param event The button click event
     */
    void OnChoseStation(wxCommandEvent& event);

    /**
     * @brief Handles the "Choose Sensor" button click
     * @param event The button click event
     */
    void OnChooseSensor(wxCommandEvent& event);

    /**
     * @brief Handles the "Show Data Files" button click
     * @param event The button click event
     */
    void OnShowDataFiles(wxCommandEvent& event);

    /**
     * @brief Handles the "View Stored Data" button click
     * @param event The button click event
     */
    void OnViewStoredData(wxCommandEvent& event);
    ///@}

    /// @name Helper functions for sensor selection
    ///@{
    /**
     * @brief Initializes the sensor database
     * @param STATION_DIRECTORY Directory for the station data
     * @param databasePath Path to the database file
     * @param apiUrl URL for the API endpoint
     * @return true if initialization was successful, false otherwise
     */
    bool InitializeSensorDatabase(const std::string& STATION_DIRECTORY, const std::string& databasePath, const std::string& apiUrl);

    /**
     * @brief Loads sensor data from the database
     * @param databasePath Path to the database file
     * @param sensors JSON value to store the loaded sensor data
     * @return true if data was successfully loaded, false otherwise
     */
    bool LoadSensorsData(const std::string& databasePath, Json::Value& sensors);

    /**
     * @brief Downloads data for all sensors
     * @param STATION_DIRECTORY Directory for the station data
     * @param sensors JSON value containing sensor information
     * @param failedSensors Vector to store IDs of sensors that failed to download
     */
    void DownloadAllSensorsData(const std::string& STATION_DIRECTORY, const Json::Value& sensors, std::vector<std::string>& failedSensors);

    /**
     * @brief Reports sensors that failed to download
     * @param failedSensors Vector containing IDs of sensors that failed to download
     */
    void ReportFailedDownloads(const std::vector<std::string>& failedSensors);

    /**
     * @brief Ensures the selected sensor data is available
     * @param STATION_DIRECTORY Directory for the station data
     * @param failedSensors Vector containing IDs of sensors that failed to download
     */
    void EnsureSelectedSensorData(const std::string& STATION_DIRECTORY, const std::vector<std::string>& failedSensors);

    /**
     * @brief Downloads data for a specific sensor
     * @param sensorId ID of the sensor
     * @param STATION_DIRECTORY Directory for the station data
     * @param failedSensors Vector to store IDs of sensors that failed to download
     * @param mutex Mutex for thread synchronization
     * @return true if download was successful, false otherwise
     */
    bool DownloadSensorData(int sensorId, const std::string& STATION_DIRECTORY, std::vector<std::string>& failedSensors, std::mutex& mutex);
    ///@}

    /// @name Helper functions for viewing stored data
    ///@{
    /**
     * @brief Checks if downloaded data is available
     * @return true if data is available, false otherwise
     */
    bool CheckForDownloadedData();

    /**
     * @brief Gets a list of numeric directories in the specified path
     * @param directory Path to search for numeric directories
     * @return Vector of directory IDs as integers
     */
    std::vector<int> GetNumericDirectories(const std::string& directory);

    /**
     * @brief Filters JSON items by existing directory IDs
     * @param allItems JSON value containing all items
     * @param dirIds Vector of directory IDs
     * @return Filtered JSON value
     */
    Json::Value FilterItemsByExistingDirectories(const Json::Value& allItems, const std::vector<int>& dirIds);

    /**
     * @brief Prompts the user to select a station
     * @param filteredStations JSON value containing filtered station data
     * @return true if a station was selected, false otherwise
     */
    bool SelectStation(Json::Value& filteredStations);

    /**
     * @brief Prompts the user to select a sensor
     * @return true if a sensor was selected, false otherwise
     */
    bool SelectSensor();
    ///@}
};
#endif