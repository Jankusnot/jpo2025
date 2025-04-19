#ifndef STATION_SELECTION_DIALOG_H
#define STATION_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <wx/listbox.h>
#include <vector>
#include <json/json.h>

class StationSelectionDialog : public wxDialog {
private:
    wxListBox* stationListBox;
    std::vector<int> stationIds;
    int selectedId;
    wxString selectedName;

public:
    StationSelectionDialog(wxWindow* parent, const Json::Value& stations);
    void OnDoubleClick(wxCommandEvent& event);
    bool GetSelectedStation(int& id, wxString& name);
};

#endif