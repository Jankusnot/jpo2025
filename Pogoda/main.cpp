#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include <wx/msgdlg.h>
#include <wx/listbox.h>
#include <wx/stattext.h>

#include "ChartPanel.h"
#include "InfoPanel.h"
#include "SidePanel.h"

class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Air quality APP", wxDefaultPosition, wxSize(1400, 800)) {
        SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        SidePanel* sidePanel = new SidePanel(this);

        wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
        ChartPanel* chartPanel = new ChartPanel(this);
        InfoPanel* infoPanel = new InfoPanel(this);

        sidePanel->ConnectToPanels(infoPanel, chartPanel);

        rightSizer->Add(chartPanel, 0, wxEXPAND | wxALL, 5);
        rightSizer->Add(infoPanel, 1, wxEXPAND | wxALL, 5);

        mainSizer->Add(sidePanel, 0, wxEXPAND);
        mainSizer->Add(rightSizer, 1, wxEXPAND);

        SetSizer(mainSizer);
        Centre();
    }
};

class MyApp : public wxApp {
private:
    wxLocale* m_locale = nullptr;

public:
    bool OnInit() override {
        curl_global_init(CURL_GLOBAL_DEFAULT);
        wxFont::AddPrivateFont("C:\\Windows\\Fonts\\arial.ttf");

        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }

    int OnExit() override {
        delete m_locale;
        curl_global_cleanup();
        return wxApp::OnExit();
    }
};

wxIMPLEMENT_APP(MyApp);