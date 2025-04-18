#include "SensorSelectionDialog.h"

SensorSelectionDialog::SensorSelectionDialog(wxWindow* parent, const Json::Value& sensors)
    : wxDialog(parent, wxID_ANY, "Select Sensor", wxDefaultPosition, wxSize(500, 400)) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create listbox to display sensors
    sensorListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(480, 300));
    mainSizer->Add(sensorListBox, 1, wxEXPAND | wxALL, 10);

    // Add OK and Cancel buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 10);
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER, 10);

    SetSizer(mainSizer);

    // Load sensors into the listbox
    if (!sensors.isNull() && sensors.isArray()) {
        for (Json::Value::ArrayIndex i = 0; i < sensors.size(); i++) {
            const Json::Value& sensor = sensors[i];
            if (sensor.isMember("id") && sensor.isMember("param") && sensor["param"].isMember("paramName")) {
                wxString paramName = wxString::FromUTF8(sensor["param"]["paramName"].asString());
                int id = sensor["id"].asInt();
                // Store the sensor ID corresponding to this listbox item
                sensorIds.push_back(id);
                sensorListBox->Append(paramName);
            }
        }
    }

    // Bind events
    sensorListBox->Bind(wxEVT_LISTBOX_DCLICK, &SensorSelectionDialog::OnDoubleClick, this);
}

void SensorSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

bool SensorSelectionDialog::GetSelectedSensor(int& id, wxString& paramName) {
    int selection = sensorListBox->GetSelection();
    if (selection != wxNOT_FOUND && selection < sensorIds.size()) {
        id = sensorIds[selection];
        paramName = sensorListBox->GetString(selection);
        return true;
    }
    return false;
}