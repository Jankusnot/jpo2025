#ifndef STATION_SELECTION_DIALOG_H
#define STATION_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <wx/fileconf.h>
#include <wx/filename.h>
#include <json/json.h>
#include <algorithm>
#include <cmath>

/**
 * @file StationSelectionDialog.h
 * @brief Dialog for selecting a station sorted by distance from reference location
 */

 /**
  * @struct StationInfo
  * @brief Structure for storing station information with geographical coordinates and calculated distance
  */
struct StationInfo {
    int id;             /**< Unique identifier for the station */
    wxString name;      /**< Name of the station */
    double latitude;    /**< Geographical latitude of the station */
    double longitude;   /**< Geographical longitude of the station */
    double distance;    /**< Calculated distance from reference location in kilometers */
};

/**
 * @class StationSelectionDialog
 * @brief A dialog that displays a list of stations sorted by distance from a reference location
 *
 * This dialog presents a list of stations parsed from JSON data, calculates their
 * distance from a reference location (loaded from config or using defaults),
 * sorts them by distance, and allows the user to select one.
 */
class StationSelectionDialog : public wxDialog {
private:
    /**
     * @brief ListBox widget that displays the station names and distances
     */
    wxListBox* stationListBox;

    /**
     * @brief Vector storing the information about stations
     */
    std::vector<StationInfo> stations;

    /**
     * @brief Reference latitude used for distance calculation
     */
    double refLatitude;

    /**
     * @brief Reference longitude used for distance calculation
     */
    double refLongitude;

    /**
     * @brief Reads reference coordinates from config.ini file
     *
     * If coordinates are not found in the config file, default values are used.
     */
    void ReadReferenceCoordinates();

    /**
     * @brief Calculates distance between two geographical coordinates using the Haversine formula
     *
     * @param lat1 Latitude of first location in degrees
     * @param lon1 Longitude of first location in degrees
     * @param lat2 Latitude of second location in degrees
     * @param lon2 Longitude of second location in degrees
     * @return double Distance in kilometers
     */
    double CalculateDistance(double lat1, double lon1, double lat2, double lon2);

    /**
     * @brief Event handler for double-click events on the station list
     *
     * Closes the dialog with OK status when a station is double-clicked.
     *
     * @param event The wxCommandEvent associated with the double-click
     */
    void OnDoubleClick(wxCommandEvent& event);

public:
    /**
     * @brief Constructor that creates a station selection dialog
     *
     * Creates a dialog with a list of stations parsed from JSON data and
     * sorted by distance from a reference location.
     *
     * @param parent Pointer to the parent window
     * @param stationsData JSON value containing the station data array
     */
    StationSelectionDialog(wxWindow* parent, const Json::Value& stationsData);

    /**
     * @brief Gets information about the selected station
     *
     * Retrieves the ID and name of the selected station.
     *
     * @param[out] id Reference to store the ID of the selected station
     * @param[out] name Reference to store the name of the selected station
     * @return bool True if a station was selected, false otherwise
     */
    bool GetSelectedStation(int& id, wxString& name);
};

#endif // STATION_SELECTION_DIALOG_H