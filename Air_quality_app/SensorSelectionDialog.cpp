/**
 * @file SensorSelectionDialog.cpp
 * @brief Implementation of the SensorSelectionDialog class
 */

#include "SensorSelectionDialog.h"

 /**
  * @brief Constructor that creates a dialog for selecting a sensor from JSON data
  *
  * Creates a modal dialog with a listbox containing sensor names extracted from
  * the provided JSON data, along with OK and Cancel buttons.
  *
  * The constructor parses the JSON data, extracts sensor information (ID and parameter name),
  * and populates the listbox with the parameter names while storing the corresponding IDs.
  *
  * @param parent Pointer to the parent window
  * @param sensors JSON value containing an array of sensor objects, each with an "id" field
  *                and a "param" object that contains a "paramName" field
  */
SensorSelectionDialog::SensorSelectionDialog(wxWindow* parent, const Json::Value& sensors)
    : wxDialog(parent, wxID_ANY, "Select Sensor", wxDefaultPosition, wxSize(400, 300)) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create listbox to display sensors
    sensorListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(350, 200));
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

/**
 * @brief Handler for double-click events that confirms selection and closes the dialog
 *
 * When a sensor is double-clicked, the dialog is closed with an OK status.
 * The selected sensor can then be retrieved using GetSelectedSensor().
 *
 * @param event The wxCommandEvent associated with the double-click
 */
void SensorSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

/**
 * @brief Retrieves selected sensor information if a valid selection exists
 *
 * This method should be called after the dialog has been closed with wxID_OK status
 * to retrieve information about the sensor that was selected.
 *
 * @param[out] id Reference to store the ID of the selected sensor
 * @param[out] paramName Reference to store the parameter name of the selected sensor
 * @return bool True if a sensor was selected and valid information was retrieved,
 *              false if no selection was made or the selection is invalid
 */
bool SensorSelectionDialog::GetSelectedSensor(int& id, wxString& paramName) {
    int selection = sensorListBox->GetSelection();
    if (selection != wxNOT_FOUND && selection < sensorIds.size()) {
        id = sensorIds[selection];
        paramName = sensorListBox->GetString(selection);
        return true;
    }
    return false;
}