#include "FileSelectionDialog.h"

// Constructor that creates a dialog with a list of files to select from
FileSelectionDialog::FileSelectionDialog(wxWindow* parent, const std::vector<std::string>& files)
    : wxDialog(parent, wxID_ANY, "Select Data File", wxDefaultPosition, wxSize(300, 400)) {
    wxBoxSizer* mainSizer = new wxBoxSizer(wxVERTICAL);

    // Create a descriptive text label at the top of the dialog
    wxStaticText* label = new wxStaticText(this, wxID_ANY, "Available data files:");
    mainSizer->Add(label, 0, wxALL, 10); // Adds label with 10px margin on all sides

    // Create listbox to display files
    fileListBox = new wxListBox(this, wxID_ANY, wxDefaultPosition, wxSize(280, 300));
    mainSizer->Add(fileListBox, 1, wxEXPAND | wxLEFT | wxRIGHT, 10);

    // Add OK and Cancel buttons
    wxBoxSizer* buttonSizer = new wxBoxSizer(wxHORIZONTAL);
    buttonSizer->Add(new wxButton(this, wxID_OK, "Select"), 0, wxALL, 10);
    buttonSizer->Add(new wxButton(this, wxID_CANCEL, "Cancel"), 0, wxALL, 10);
    mainSizer->Add(buttonSizer, 0, wxALIGN_CENTER | wxBOTTOM, 10);

    SetSizer(mainSizer);

    // Populate the listbox with file names from the input vector
    for (const auto& file : files) {
        fileListBox->Append(wxString(file));
    }

    // Bind events
    fileListBox->Bind(wxEVT_LISTBOX_DCLICK, &FileSelectionDialog::OnDoubleClick, this);
    Bind(wxEVT_BUTTON, &FileSelectionDialog::OnOK, this, wxID_OK);
}

// Handler for double-click events on the file list that selects a file and closes the dialog
void FileSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    if (fileListBox->GetSelection() != wxNOT_FOUND) {
        selectedFile = fileListBox->GetStringSelection();
        EndModal(wxID_OK);
    }
}

// Handler for OK button clicks that validates selection and closes the dialog
void FileSelectionDialog::OnOK(wxCommandEvent& event) {
    if (fileListBox->GetSelection() != wxNOT_FOUND) {
        selectedFile = fileListBox->GetStringSelection();
        EndModal(wxID_OK);
    }
    else {
        wxMessageBox("Please select a file.", "No Selection", wxOK | wxICON_INFORMATION);
    }
}

// Getter method that returns the name of the selected file
wxString FileSelectionDialog::GetSelectedFile() const {
    return selectedFile;
}