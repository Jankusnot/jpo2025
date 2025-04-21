#pragma once

#include <wx/wx.h>
#include <vector>
#include <string>

class FileSelectionDialog : public wxDialog {
public:
    FileSelectionDialog(wxWindow* parent, const std::vector<std::string>& files);
    wxString GetSelectedFile() const;

private:
    wxListBox* fileListBox;
    wxString selectedFile;

    void OnDoubleClick(wxCommandEvent& event);
    void OnOK(wxCommandEvent& event);
};