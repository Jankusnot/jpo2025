#include "StationSelectionDialog.h"

StationSelectionDialog::StationSelectionDialog(wxWindow* parent, const Json::Value& stations)
    : wxDialog(parent, wxID_ANY, "Select Station", wxDefaultPosition, wxSize(500, 400)) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create listbox to display stations
    stationListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(480, 300));
    mainSizer->Add(stationListBox, 1, wxEXPAND | wxALL, 10);

    // Add OK and Cancel buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(this, wxID_OK, "OK"), 0, wxALL, 10);
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER, 10);

    SetSizer(mainSizer);

    // Load stations into the listbox
    if (!stations.isNull() && stations.isArray()) {
        for (Json::Value::ArrayIndex i = 0; i < stations.size(); i++) {
            const Json::Value& station = stations[i];
            if (station.isMember("id") && station.isMember("stationName")) {
                // Convert to wxString using proper UTF-8 handling
                wxString name = wxString::FromUTF8(station["stationName"].asString().c_str());
                int id = station["id"].asInt();
                // Store the station ID corresponding to this listbox item
                stationIds.push_back(id);
                stationListBox->Append(name);
            }
        }
    }

    // Bind events
    stationListBox->Bind(wxEVT_LISTBOX_DCLICK, &StationSelectionDialog::OnDoubleClick, this);
}

void StationSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

bool StationSelectionDialog::GetSelectedStation(int& id, wxString& name) {
    int selection = stationListBox->GetSelection();
    if (selection != wxNOT_FOUND && selection < stationIds.size()) {
        id = stationIds[selection];
        name = stationListBox->GetString(selection);
        return true;
    }
    return false;
}