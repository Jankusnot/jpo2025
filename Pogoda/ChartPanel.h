#ifndef CHART_PANEL_H
#define CHART_PANEL_H

#include <wx/wx.h>
#include <vector>
#include <utility>
#include <limits>
#include <algorithm>

class ChartPanel : public wxPanel {
private:
    std::vector<std::pair<wxString, double>> chartData;
    wxString chartTitle;
    wxString xAxisLabel;
    wxString yAxisLabel;
    bool hasData;

    void OnPaint(wxPaintEvent& event);

public:
    ChartPanel(wxWindow* parent);

    void SetChartData(const std::vector<std::pair<wxString, double>>& data,
        const wxString& title,
        const wxString& xLabel,
        const wxString& yLabel);

    void ClearChart();
};

#endif // CHART_PANEL_H