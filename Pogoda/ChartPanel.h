#ifndef CHART_PANEL_H
#define CHART_PANEL_H

#include <wx/wx.h>
#include <algorithm>
#include <vector>
#include <utility>
#include <limits>

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
    void SetChartData(const std::vector<std::pair<wxString, double>>& data, const wxString& title, const wxString& xLabel, const wxString& yLabel);

private:
    //Helper functions for OnPaint function
    void DrawNoDataMessage(wxPaintDC& dc);
    void SetupChartDimensions(int& chartLeft, int& chartTop, int& chartWidth, int& chartHeight, int& chartBottom, int& chartRight);
    void DrawChartBackground(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight);
    void CalculateValueRange(double& minValue, double& maxValue);
    void DrawChartTitle(wxPaintDC& dc, int chartLeft, int chartWidth, int chartTop);
    void DrawAxisLabels(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight, int chartBottom);
    void DrawYAxisTicksAndGrid(wxPaintDC& dc, int chartLeft, int chartRight, int chartBottom, int chartHeight, double minValue, double maxValue);
    void DrawXAxisTicksAndGrid(wxPaintDC& dc, int chartLeft, int chartTop, int chartBottom, int chartWidth);
    void DrawDataPointsAndLines(wxPaintDC& dc, int chartLeft, int chartBottom, int chartWidth, int chartHeight, double minValue, double maxValue);
};

#endif