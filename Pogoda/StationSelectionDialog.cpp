#include "StationSelectionDialog.h"

// Constructor that creates a dialog for selecting a station from JSON data and sorts by distance
StationSelectionDialog::StationSelectionDialog(wxWindow * parent, const Json::Value & stations)
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

    // Read reference coordinates from config.ini
    ReadReferenceCoordinates();

    // Load and sort stations by distance
    if (!stations.isNull() && stations.isArray()) {
        for (Json::Value::ArrayIndex i = 0; i < stations.size(); i++) {
            const Json::Value& station = stations[i];
            if (station.isMember("id") && station.isMember("stationName") &&
                station.isMember("gegrLat") && station.isMember("gegrLon")) {

                // Creates container for station information
                StationInfo info; 
                info.id = station["id"].asInt();
                info.name = wxString::FromUTF8(station["stationName"].asString().c_str());
                info.latitude = std::stod(station["gegrLat"].asString());
                info.longitude = std::stod(station["gegrLon"].asString());

                // Calculate distance from reference point
                info.distance = CalculateDistance(refLatitude, refLongitude, info.latitude, info.longitude);

                this->stations.push_back(info);
            }
        }

        // Sort stations by distance
        std::sort(this->stations.begin(), this->stations.end(),
            [](const StationInfo& a, const StationInfo& b) {
                return a.distance < b.distance;
            });

        // Add sorted stations to the listbox
        for (const auto& station : this->stations) {
            // Format: Station Name (Distance: X.XX km)
            wxString displayStr = wxString::Format("%s (Distance: %.2f km)",
                station.name, station.distance
            );
            stationListBox->Append(displayStr);
        }
    }

    // Bind events
    stationListBox->Bind(wxEVT_LISTBOX_DCLICK, &StationSelectionDialog::OnDoubleClick, this); // Connects double-click event to handler
}

// Reads user location coordinates from config file or sets defaults if not found
void StationSelectionDialog::ReadReferenceCoordinates() {
    refLatitude = 52.11433;
    refLongitude = 19.42367;

    // Create a file config object pointing to config.ini in the application directory
    wxString configPath = wxFileName::GetCwd() + wxFileName::GetPathSeparator() + "config.ini";
    wxFileConfig config(wxEmptyString, wxEmptyString, configPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

    // Read coordinates
    config.Read("/Location/Latitude", &refLatitude);
    config.Read("/Location/Longitude", &refLongitude);
}

// Calculates distance between two geographical coordinates using the Haversine formula
double StationSelectionDialog::CalculateDistance(double lat1, double lon1, double lat2, double lon2) {
    // Convert degrees to radians
    const double toRad = M_PI / 180.0; // Conversion constant
    lat1 *= toRad;
    lon1 *= toRad;
    lat2 *= toRad;
    lon2 *= toRad;

    // Haversine formula to calculate distance between two points on Earth
    const double R = 6371.0; // Earth radius in kilometers
    double dLat = lat2 - lat1;
    double dLon = lon2 - lon1;

    double a = sin(dLat / 2) * sin(dLat / 2) + cos(lat1) * cos(lat2) * sin(dLon / 2) * sin(dLon / 2);
    double c = 2 * atan2(sqrt(a), sqrt(1 - a));
    double distance = R * c;

    return distance;
}

// Handler for double-click events that confirms selection and closes the dialog
void StationSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

// Retrieves selected station information if a valid selection exists
bool StationSelectionDialog::GetSelectedStation(int& id, wxString& name) {
    int selection = stationListBox->GetSelection();
    if (selection != wxNOT_FOUND && selection < stations.size()) {
        id = stations[selection].id;
        name = stations[selection].name;
        return true;
    }
    return false;
}