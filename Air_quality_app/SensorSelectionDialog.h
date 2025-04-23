#ifndef SENSOR_SELECTION_DIALOG_H
#define SENSOR_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <vector>
#include <json/json.h>

/**
 * @file SensorSelectionDialog.h
 * @brief Dialog for selecting a sensor from a list
 */

 /**
  * @class SensorSelectionDialog
  * @brief A dialog that displays a list of sensors and allows the user to select one
  *
  * This dialog presents a list of sensors parsed from JSON data and provides
  * functionality for selecting a sensor either by double-clicking on it
  * or by selecting it and clicking the OK button.
  */
class SensorSelectionDialog : public wxDialog {
private:
    /**
     * @brief ListBox widget that displays the sensor names
     */
    wxListBox* sensorListBox;

    /**
     * @brief Vector storing the IDs of sensors in the same order as they appear in the list
     */
    std::vector<int> sensorIds;

    /**
     * @brief Stores the ID of the selected sensor
     */
    int selectedId;

    /**
     * @brief Stores the parameter name of the selected sensor
     */
    wxString selectedParamName;

public:
    /**
     * @brief Constructor that creates a sensor selection dialog
     *
     * Creates a dialog with a list of sensors parsed from JSON data.
     *
     * @param parent Pointer to the parent window
     * @param sensors JSON value containing the sensor data array
     */
    SensorSelectionDialog(wxWindow* parent, const Json::Value& sensors);

    /**
     * @brief Event handler for double-click events on the sensor list
     *
     * Closes the dialog with OK status when a sensor is double-clicked.
     *
     * @param event The wxCommandEvent associated with the double-click
     */
    void OnDoubleClick(wxCommandEvent& event);

    /**
     * @brief Gets information about the selected sensor
     *
     * Retrieves the ID and parameter name of the selected sensor.
     *
     * @param[out] id Reference to store the ID of the selected sensor
     * @param[out] paramName Reference to store the parameter name of the selected sensor
     * @return bool True if a sensor was selected, false otherwise
     */
    bool GetSelectedSensor(int& id, wxString& paramName);
};

#endif // SENSOR_SELECTION_DIALOG_H