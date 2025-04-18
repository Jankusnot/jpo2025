#ifndef SIDE_PANEL_H
#define SIDE_PANEL_H


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
    void OnViewDownloadedData(wxCommandEvent& event);
};
#endif