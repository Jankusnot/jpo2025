/**
 * @file FileSelectionDialog.cpp
 * @brief Implementation of the FileSelectionDialog class
 */

#include "FileSelectionDialog.h"

 /**
  * @brief Constructor that creates a dialog with a list of files to select from
  *
  * Creates a modal dialog with a listbox containing file names, and OK/Cancel buttons.
  * The dialog allows users to select a file either by double-clicking or using the OK button.
  *
  * @param parent Pointer to the parent window
  * @param files Vector of strings containing the file names to display
  */
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

/**
 * @brief Handler for double-click events on the file list
 *
 * When a file is double-clicked, it is selected and the dialog is closed with an OK status.
 * The selected file can then be retrieved using GetSelectedFile().
 *
 * @param event The wxCommandEvent associated with the double-click
 */
void FileSelectionDialog::OnDoubleClick(wxCommandEvent& event) {
    if (fileListBox->GetSelection() != wxNOT_FOUND) {
        selectedFile = fileListBox->GetStringSelection();
        EndModal(wxID_OK);
    }
}

/**
 * @brief Handler for OK button clicks
 *
 * Validates that a file is selected before closing the dialog with an OK status.
 * If no file is selected, displays a message asking the user to make a selection.
 *
 * @param event The wxCommandEvent associated with the button click
 */
void FileSelectionDialog::OnOK(wxCommandEvent& event) {
    if (fileListBox->GetSelection() != wxNOT_FOUND) {
        selectedFile = fileListBox->GetStringSelection();
        EndModal(wxID_OK);
    }
    else {
        wxMessageBox("Please select a file.", "No Selection", wxOK | wxICON_INFORMATION);
    }
}

/**
 * @brief Returns the name of the selected file
 *
 * This method should be called after the dialog has been closed with wxID_OK status
 * to retrieve the name of the file that was selected.
 *
 * @return wxString The name of the selected file, or empty string if no file was selected
 */
wxString FileSelectionDialog::GetSelectedFile() const {
    return selectedFile;
}