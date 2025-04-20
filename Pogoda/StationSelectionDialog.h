#ifndef STATION_SELECTION_DIALOG_H
#define STATION_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <wx/listbox.h>
#include <wx/config.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <vector>
#include <json/json.h>
#include <cmath>
#include <algorithm>
#include <string>

struct StationInfo {
    int id;
    wxString name;
    double latitude;
    double longitude;
    double distance;  // Distance from reference point
};

class StationSelectionDialog : public wxDialog {
private:
    wxListBox* stationListBox;
    std::vector<StationInfo> stations;

    // Reference point coordinates
    double refLatitude;
    double refLongitude;

    // Reads reference coordinates from ini file
    void ReadReferenceCoordinates();

    // Calculates distance between two geographic points
    double CalculateDistance(double lat1, double lon1, double lat2, double lon2);

public:
    StationSelectionDialog(wxWindow* parent, const Json::Value& stationsData);
    void OnDoubleClick(wxCommandEvent& event);
    bool GetSelectedStation(int& id, wxString& name);
};

#endif