/**
 * @file InfoPanel.cpp
 * @brief Implementation of the InfoPanel class
 */

#include "InfoPanel.h"

 /**
  * @brief Constructor that initializes the panel with text components for displaying data analysis
  * @param parent Pointer to the parent window
  *
  * Creates a panel with a title and text area for displaying statistical
  * information about air quality measurements.
  */
InfoPanel::InfoPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1200, 200)) {
    SetBackgroundColour(wxColour(240, 240, 240));

    // Create vertical sizer to arrange text elements
    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    // Create and configure title text with bold font
    titleText = new wxStaticText(this, wxID_ANY, "Data analysis", wxDefaultPosition, wxDefaultSize);
    titleText->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    // Create statistics text control that will display the analysis results
    statsText = new wxStaticText(this, wxID_ANY, "", wxDefaultPosition, wxDefaultSize);
    statsText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    // Add title text and statistics to the panel
    sizer->Add(titleText, 0, wxALL, 10);
    sizer->Add(statsText, 1, wxALL | wxEXPAND, 10);

    // Apply the sizer to layout the panel
    SetSizer(sizer);
}

/**
 * @brief Updates the displayed statistics with formatted data
 * @param key Pollutant identifier being displayed
 * @param max Maximum measured value of the pollutant
 * @param min Minimum measured value of the pollutant
 * @param avg Average measured value of the pollutant
 * @param trend String describing the trend of the pollutant values
 *
 * Formats and displays the statistical information about a pollutant including
 * its maximum, minimum, and average values with appropriate units.
 * Also displays trend information that indicates whether values are increasing,
 * decreasing, or stable over time.
 */
void InfoPanel::UpdateStatistics(const wxString& key, double max, double min, double avg, const wxString& trend) {
    wxString unit = wxT("\u03BCg / m\u00B3");  // Unicode for micrograms per meter cubed

    wxString content = wxString::Format("%s:\nmaximum: %.2f %s,\nminimum: %.2f %s,\naverage: %.2f %s,\ntrend: %s",
        key, max, unit, min, unit, avg, unit, trend
    );

    statsText->SetLabel(content);
    // Recalculate layout to accommodate the new text
    Layout();
}