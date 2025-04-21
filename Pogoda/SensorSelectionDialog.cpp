#include "SensorSelectionDialog.h"

// Constructor that creates a dialog for selecting a sensor from JSON data
SensorSelectionDialog::SensorSelectionDialog(wxWindow* parent, const Json::Value& sensors)
    : wxDialog(parent, wxID_ANY, "Select Sensor", wxDefaultPosition, wxSize(300, 200)) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create listbox to display sensors
    sensorListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(280, 150));
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
                sensorIds.push_back(id);
                sensorListBox->Append(paramName);
            }
        }
    }

    // Bind events
    sensorListBox->Bind(wxEVT_LISTBOX_DCLICK, &SensorSelectionDialog::OnDoubleClick, this);
}

// Handler for double-click events that confirms selection and closes the dialog
void SensorSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

// Retrieves selected sensor information if a valid selection exists
bool SensorSelectionDialog::GetSelectedSensor(int& id, wxString& paramName) {
    int selection = sensorListBox->GetSelection();
    if (selection != wxNOT_FOUND && selection < sensorIds.size()) {
        id = sensorIds[selection];
        paramName = sensorListBox->GetString(selection);
        return true;
    }
    return false;
}