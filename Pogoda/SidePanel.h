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

class SidePanel : public wxPanel {
private:
    // UI Components
    wxButton* btn1;
    wxButton* btn2;
    wxButton* btn3;
    wxButton* btn4;
    wxStaticText* selectedStationText;
    wxStaticText* selectedSensorText;
    wxStaticText* selectedFileText;

    // Selection state
    int selectedStationId;
    wxString selectedStationName;
    int selectedSensorId;
    wxString selectedSensorParamName;
    wxString selectedFileName;

    // Panel references
    InfoPanel* infoPanel;
    ChartPanel* chartPanel;

    // Constants
    const std::string STATIONS_API_URL = "http://api.gios.gov.pl/pjp-api/rest/station/findAll";
    const std::string SENSORS_API_URL = "http://api.gios.gov.pl/pjp-api/rest/station/sensors/";
    const std::string SENSOR_API_URL = "http://api.gios.gov.pl/pjp-api/rest/data/getData/";
    const std::string DATABASE_DIRECTORRY = "measurements";
    const std::string DATABASE_STATIONS = DATABASE_DIRECTORRY + "/stations.json";

public:
    SidePanel(wxWindow* parent);
    void ConnectToPanels(InfoPanel* info, ChartPanel* chart);

private:
    // UI initialization
    void InitializeUI();
    void BindEvents();

    // Status display updates
    void UpdateSelectedStationDisplay();
    void UpdateSelectedSensorDisplay();
    void UpdateSelectedFileDisplay();

    //Button binded functions
    void OnChoseStation(wxCommandEvent& event);
    void OnChooseSensor(wxCommandEvent& event);
    void OnShowDataFiles(wxCommandEvent& event);
    void OnViewStoredData(wxCommandEvent& event);

    //Helper functions for OnChooseSensor
    bool InitializeSensorDatabase(const std::string& STATION_DIRECTORY, const std::string& databasePath, const std::string& apiUrl);
    bool LoadSensorsData(const std::string& databasePath, Json::Value& sensors);
    void DownloadAllSensorsData(const std::string& STATION_DIRECTORY, const Json::Value& sensors, std::vector<std::string>& failedSensors);
    void ReportFailedDownloads(const std::vector<std::string>& failedSensors);
    void EnsureSelectedSensorData(const std::string& STATION_DIRECTORY, const std::vector<std::string>& failedSensors);
    bool DownloadSensorData(int sensorId, const std::string& STATION_DIRECTORY, std::vector<std::string>& failedSensors, std::mutex& mutex);

    //Helper functions for OnViewStoredData
    bool CheckForDownloadedData();
    std::vector<int> GetNumericDirectories(const std::string& directory);
    Json::Value FilterItemsByExistingDirectories(const Json::Value& allItems, const std::vector<int>& dirIds);
    bool SelectStation(Json::Value& filteredStations);
    bool SelectSensor();
};
#endif