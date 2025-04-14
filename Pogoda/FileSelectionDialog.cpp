#include "FileSelectionDialog.h"

FileSelectionDialog::FileSelectionDialog(wxWindow* parent, const std::vector<std::string>& files)
    : wxDialog(parent, wxID_ANY, "Select Data File", wxDefaultPosition, wxSize(300, 400)) {

    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    wxStaticText* label = new wxStaticText(this, wxID_ANY, "Available data files:");
    mainSizer->Add(label, 0, wxALL, 10);

    fileListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(280, 300));

    // Add files to the list box
    for (const auto& file : files) {
        fileListBox->Append(wxString(file));
    }

    mainSizer->Add(fileListBox, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Create buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    wxButton* okButton = new wxButton(this, wxID_OK, "Select");
    wxButton* cancelButton = new wxButton(this, wxID_CANCEL, "Cancel");

    buttonSizer->Add(okButton, 0, wxALL, 10);
    buttonSizer->Add(cancelButton, 0, wxALL, 10);

    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    SetSizer(mainSizer);

    // Bind events
    fileListBox->Bind(wxEVT_LISTBOX_DCLICK, [this](wxCommandEvent&) {
        if (fileListBox->GetSelection() != wxNOT_FOUND) {
            selectedFile = fileListBox->GetStringSelection();
            EndModal(wxID_OK);
        }
        });

    okButton->Bind(wxEVT_BUTTON, [this](wxCommandEvent&) {
        if (fileListBox->GetSelection() != wxNOT_FOUND) {
            selectedFile = fileListBox->GetStringSelection();
            EndModal(wxID_OK);
        }
        else {
            wxMessageBox("Please select a file.", "No Selection", wxOK | wxICON_INFORMATION);
        }
        });
}

wxString FileSelectionDialog::GetSelectedFile() const {
    return selectedFile;
}