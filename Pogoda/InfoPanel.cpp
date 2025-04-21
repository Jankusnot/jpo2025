#include "InfoPanel.h"

// Constructor that initializes the panel with text components for displaying data analysis
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

// Updates the displayed statistics with formatted data including pollutant key, values and trend
void InfoPanel::UpdateStatistics(const wxString& key, double max, double min, double avg, const wxString& trend) {
    wxString unit = wxT("\u03BCg / m\u00B3");

    wxString content = wxString::Format("%s:\nmaximum: %.2f %s,\nminimum: %.2f %s,\naverage: %.2f %s,\ntrend: %s",
        key, max, unit, min, unit, avg, unit, trend
    );

    statsText->SetLabel(content);
    // Recalculate layout to accommodate the new text
    Layout();
}