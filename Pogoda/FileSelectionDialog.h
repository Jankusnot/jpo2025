#ifndef FILE_SELECTION_DIALOG_H
#define FILE_SELECTION_DIALOG_H

#include <wx/wx.h>
#include <wx/listbox.h>
#include <vector>
#include <string>

class FileSelectionDialog : public wxDialog {
private:
    wxListBox* fileListBox;
    wxString selectedFile;

public:
    FileSelectionDialog(wxWindow* parent, const std::vector<std::string>& files);

    wxString GetSelectedFile() const;
};

#endif // FILE_SELECTION_DIALOG_H