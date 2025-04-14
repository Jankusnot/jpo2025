#ifndef INFO_PANEL_H
#define INFO_PANEL_H

#include <wx/wx.h>

class InfoPanel : public wxPanel {
private:
    wxStaticText* titleText;
    wxStaticText* statsText;

public:
    InfoPanel(wxWindow* parent);

    void UpdateStatistics(const wxString& key,
        double max,
        double min,
        double avg,
        const wxString& trend);
};

#endif // INFO_PANEL_H