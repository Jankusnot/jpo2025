#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include <wx/wx.h>
#include <string>

/**
 * @file InfoPanel.h
 * @brief Header for the InfoPanel class which displays data analysis information
 */

 /**
  * @class InfoPanel
  * @brief Panel that displays statistical analysis of air quality data
  *
  * The InfoPanel class is responsible for displaying statistical information
  * about air quality measurements including maximum, minimum, average values,
  * and trend information for selected pollutants.
  */
class InfoPanel : public wxPanel {
private:
    wxStaticText* titleText; /**< Title text displayed at the top of the panel */
    wxStaticText* statsText; /**< Text control that displays the statistical information */

public:
    /**
     * @brief Constructor for the InfoPanel
     * @param parent Pointer to the parent window
     *
     * Creates and initializes the InfoPanel with text components
     * for displaying data analysis information.
     */
    InfoPanel(wxWindow* parent);

    /**
     * @brief Updates the displayed statistics with new data
     * @param key Identifier of the pollutant being displayed
     * @param max Maximum value of the pollutant
     * @param min Minimum value of the pollutant
     * @param avg Average value of the pollutant
     * @param trend String describing the trend of the pollutant values
     *
     * This method formats and displays statistical information about a pollutant,
     * including its maximum, minimum, and average values, with appropriate units.
     */
    void UpdateStatistics(const wxString& key, double max, double min, double avg, const wxString& trend);
};

#endif // INFO_PANEL_H