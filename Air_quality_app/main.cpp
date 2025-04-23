/**
 * @file main.cpp
 * @brief Main application file for the Air Quality monitoring application
 *
 * This file contains the main application window and application class
 * implementations for app.
 */

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

 /**
  * @class MyFrame
  * @brief Main application window that manages the overall UI layout
  *
  * The MyFrame class is responsible for creating and managing the main application
  * window, including all UI panels and their arrangements.
  */
class MyFrame : public wxFrame {
public:
    /**
     * @brief Constructor for the main application frame
     *
     * Creates and initializes the main window with its panels and layout.
     * Sets up the connection between panels to enable data flow.
     */
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

/**
 * @class MyApp
 * @brief Application class that handles initialization and cleanup
 *
 * The MyApp class is responsible for initializing application resources,
 * creating the main window, and cleaning up resources when the application exits.
 */
class MyApp : public wxApp {
private:
    wxLocale* m_locale = nullptr; /**< Localization support for the application */

public:
    /**
     * @brief Initializes application resources
     * @return true if initialization succeeded, false otherwise
     *
     * This method initializes the curl library for HTTP requests and
     * sets up localization. It also creates and displays the main application window.
     */
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

    /**
     * @brief Cleans up application resources
     * @return Exit code for the application
     *
     * This method frees resources used by the application including
     * the locale object and curl library resources.
     */
    int OnExit() override {
        // Clean up locale and curl resources
        delete m_locale;
        curl_global_cleanup();
        return wxApp::OnExit();
    }
};

// wxWidgets macro to declare the application entry point
wxIMPLEMENT_APP(MyApp);