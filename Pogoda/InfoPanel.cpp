#include "InfoPanel.h"

InfoPanel::InfoPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1200, 200)) {

    SetBackgroundColour(wxColour(240, 240, 240));

    wxBoxSizer* sizer = new wxBoxSizer(wxVERTICAL);

    titleText = new wxStaticText(this, wxID_ANY, "Data analysis",
        wxDefaultPosition, wxDefaultSize);
    titleText->SetFont(wxFont(11, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));

    statsText = new wxStaticText(this, wxID_ANY, "",
        wxDefaultPosition, wxDefaultSize);
    statsText->SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    sizer->Add(titleText, 0, wxALL, 10);
    sizer->Add(statsText, 1, wxALL | wxEXPAND, 10);

    SetSizer(sizer);
}

void InfoPanel::UpdateStatistics(const wxString& key,
    double max,
    double min,
    double avg,
    const wxString& trend) {

    wxString content = wxString::Format(
        "%s:\nmaximum of the values: %.2f,\nminimum of the values: %.2f,\naverage: %.2f,\ntrend: %s",
        key, max, min, avg, trend
    );

    statsText->SetLabel(content);
    Layout();
};