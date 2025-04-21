
#ifndef SENSOR_SELECTION_DIALOG_H
#define SENSOR_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <vector>
#include <json/json.h>

class SensorSelectionDialog : public wxDialog {
private:
    wxListBox* sensorListBox;
    std::vector<int> sensorIds;
    int selectedId;
    wxString selectedParamName;

public:
    SensorSelectionDialog(wxWindow* parent, const Json::Value& sensors);
    void OnDoubleClick(wxCommandEvent& event);
    bool GetSelectedSensor(int& id, wxString& paramName);
};

#endif