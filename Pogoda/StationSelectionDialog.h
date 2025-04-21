#ifndef STATION_SELECTION_DIALOG_H
#define STATION_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <json/json.h>
#include <algorithm>
#include <cmath>

struct StationInfo {
    int id;
    wxString name;
    double latitude;
    double longitude;
    double distance;
};

class StationSelectionDialog : public wxDialog {
private:
    wxListBox* stationListBox;
    std::vector<StationInfo> stations;

    double refLatitude;
    double refLongitude;

    void ReadReferenceCoordinates();
    double CalculateDistance(double lat1, double lon1, double lat2, double lon2);
    void OnDoubleClick(wxCommandEvent& event);

public:
    StationSelectionDialog(wxWindow* parent, const Json::Value& stationsData);
    bool GetSelectedStation(int& id, wxString& name);
};

#endif