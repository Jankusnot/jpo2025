#ifndef FIEL_SELECTION_DIALOG_H
#define FIEL_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <vector>
#include <string>

/**
 * @file FileSelectionDialog.h
 * @brief Dialog for selecting a file from a list
 */

 /**
  * @class FileSelectionDialog
  * @brief A dialog that displays a list of files and allows the user to select one
  *
  * This dialog presents a list of file names and provides functionality for selecting
  * a file either by double-clicking on it or by selecting it and clicking the OK button.
  * The dialog validates that a selection is made before closing with OK.
  */
class FileSelectionDialog : public wxDialog {
public:
    /**
     * @brief Constructor that creates a file selection dialog
     *
     * @param parent Pointer to the parent window
     * @param files Vector of strings containing the file names to display
     */
    FileSelectionDialog(wxWindow* parent, const std::vector<std::string>& files);

    /**
     * @brief Gets the name of the file that was selected
     *
     * @return wxString The name of the selected file, or empty string if no file was selected
     */
    wxString GetSelectedFile() const;

private:
    /**
     * @brief ListBox widget that displays the file names
     */
    wxListBox* fileListBox;

    /**
     * @brief Stores the name of the selected file
     */
    wxString selectedFile;

    /**
     * @brief Event handler for double-click events on the file list
     *
     * Selects the file under the cursor and closes the dialog with OK status.
     *
     * @param event The wxCommandEvent associated with the double-click
     */
    void OnDoubleClick(wxCommandEvent& event);

    /**
     * @brief Event handler for OK button clicks
     *
     * If a file is selected, closes the dialog with OK status.
     * Otherwise, displays a message asking the user to select a file.
     *
     * @param event The wxCommandEvent associated with the button click
     */
    void OnOK(wxCommandEvent& event);
};

#endif