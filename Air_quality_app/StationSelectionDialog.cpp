/**
 * @file StationSelectionDialog.cpp
 * @brief Implementation of the StationSelectionDialog class
 */

#include "StationSelectionDialog.h"

 /**
  * @brief Constructor that creates a dialog for selecting a station from JSON data and sorts by distance
  *
  * Creates a modal dialog with a listbox containing station names and their distances
  * from a reference location, along with OK and Cancel buttons.
  *
  * The constructor:
  * 1. Loads reference coordinates from config.ini
  * 2. Parses the JSON data to extract station information
  * 3. Calculates the distance from each station to the reference point
  * 4. Sorts stations by distance
  * 5. Populates the listbox with station names and distances
  *
  * @param parent Pointer to the parent window
  * @param stations JSON value containing an array of station objects with id, stationName,
  *                 gegrLat, and gegrLon fields
  */
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
                try {
                    // Use stringstream with classic locale to ensure correct decimal point handling
                    std::string latStr = station["gegrLat"].asString();
                    std::string lonStr = station["gegrLon"].asString();

                    std::istringstream latStream(latStr);
                    std::istringstream lonStream(lonStr);

                    // Set classic "C" locale to ensure period as decimal separator
                    latStream.imbue(std::locale::classic());
                    lonStream.imbue(std::locale::classic());

                    latStream >> info.latitude;
                    lonStream >> info.longitude;

                    if (latStream.fail() || lonStream.fail()) {
                        wxLogWarning("Failed to parse coordinates for station %s", info.name);
                        info.latitude = 0.0;
                        info.longitude = 0.0;
                    }
                }
                catch (const std::exception& e) {
                    wxLogWarning("Exception parsing coordinates for station %s: %s",
                    info.name, e.what());
                    info.latitude = 0.0;
                    info.longitude = 0.0;
                }

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

/**
 * @brief Reads user location coordinates from config file or sets defaults if not found
 *
 * Attempts to read latitude and longitude values from the config.ini file located
 * in the application directory. If the file or values are not found, default
 * coordinates for the central point of Poland (52.11433, 19.42367) are used.
 */
void StationSelectionDialog::ReadReferenceCoordinates() {
    refLatitude = 52.11433;  // Default latitude (central point of Poland)
    refLongitude = 19.42367; // Default longitude (central point of Poland)

    // Create a file config object pointing to config.ini in the application directory
    wxString configPath = wxFileName::GetCwd() + wxFileName::GetPathSeparator() + "config.ini";
    wxFileConfig config(wxEmptyString, wxEmptyString, configPath, wxEmptyString, wxCONFIG_USE_LOCAL_FILE);

    // Read coordinates
    config.Read("/Location/Latitude", &refLatitude);
    config.Read("/Location/Longitude", &refLongitude);
}

/**
 * @brief Calculates distance between two geographical coordinates using the Haversine formula
 *
 * The Haversine formula calculates the great-circle distance between two points
 * on the Earth's surface given their longitude and latitude.
 *
 * @param lat1 Latitude of first location in degrees
 * @param lon1 Longitude of first location in degrees
 * @param lat2 Latitude of second location in degrees
 * @param lon2 Longitude of second location in degrees
 * @return double Distance in kilometers
 */
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

/**
 * @brief Handler for double-click events that confirms selection and closes the dialog
 *
 * When a station is double-clicked, the dialog is closed with an OK status.
 * The selected station can then be retrieved using GetSelectedStation().
 *
 * @param event The wxCommandEvent associated with the double-click
 */
void StationSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    EndModal(wxID_OK);
}

/**
 * @brief Retrieves selected station information if a valid selection exists
 *
 * This method should be called after the dialog has been closed with wxID_OK status
 * to retrieve information about the station that was selected.
 *
 * @param[out] id Reference to store the ID of the selected station
 * @param[out] name Reference to store the name of the selected station
 * @return bool True if a station was selected and valid information was retrieved,
 *              false if no selection was made or the selection is invalid
 */
bool StationSelectionDialog::GetSelectedStation(int& id, wxString& name) {
    int selection = stationListBox->GetSelection();
    if (selection != wxNOT_FOUND && selection < stations.size()) {
        id = stations[selection].id;
        name = stations[selection].name;
        return true;
    }
    return false;
}