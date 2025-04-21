#include <wx/wx.h>
#include <wx/sizer.h>
#include <wx/font.h>
#include <wx/msgdlg.h>
#include <wx/listbox.h>
#include <wx/stattext.h>
#include <wx/intl.h>
#include <curl/curl.h>

#include "ChartPanel.h"
#include "InfoPanel.h"
#include "SidePanel.h"

// Main application window that manages the overall UI layout
class MyFrame : public wxFrame {
public:
    MyFrame() : wxFrame(nullptr, wxID_ANY, "Air quality APP", wxDefaultPosition, wxSize(1550, 800)) {
        SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

        // Main horizontal layout container
        wxBoxSizer* mainSizer = new wxBoxSizer(wxHORIZONTAL);
        SidePanel* sidePanel = new SidePanel(this);

        // Vertical container for right side components
        wxBoxSizer* rightSizer = new wxBoxSizer(wxVERTICAL);
        ChartPanel* chartPanel = new ChartPanel(this);
        InfoPanel* infoPanel = new InfoPanel(this);

        // Connect panels to enable data flow between components
        sidePanel->ConnectToPanels(infoPanel, chartPanel);

        // Add right side components to vertical container
        rightSizer->Add(chartPanel, 0, wxEXPAND | wxALL, 5);
        rightSizer->Add(infoPanel, 1, wxEXPAND | wxALL, 5);

        // Add side panel and right components to main layout
        mainSizer->Add(sidePanel, 0, wxEXPAND);
        mainSizer->Add(rightSizer, 1, wxEXPAND);

        // Set the layout manager and center the window
        SetSizer(mainSizer);
        Centre();
    }
};

// Application class that handles initialization and cleanup
class MyApp : public wxApp {
private:
    wxLocale* m_locale = nullptr;

public:
    bool OnInit() override {
        // Initialize curl library for HTTP requests
        if (curl_global_init(CURL_GLOBAL_DEFAULT) != CURLE_OK) {
            wxMessageBox("Failed to initialize curl", "Error", wxICON_ERROR);
            return false;
        }

        m_locale = new wxLocale(wxLANGUAGE_DEFAULT);

        // Create and display the main application window
        MyFrame* frame = new MyFrame();
        frame->Show();
        return true;
    }

    int OnExit() override {
        // Clean up locale and curl resources
        delete m_locale;
        curl_global_cleanup();
        return wxApp::OnExit();
    }
};

// wxWidgets macro to declare the application entry point
wxIMPLEMENT_APP(MyApp);