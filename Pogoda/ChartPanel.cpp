#include "ChartPanel.h"

ChartPanel::ChartPanel(wxWindow* parent)
    : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1200, 600)) {
    SetBackgroundColour(*wxWHITE);
    hasData = false;

    // Bind paint event
    Bind(wxEVT_PAINT, &ChartPanel::OnPaint, this);
}

void ChartPanel::SetChartData(const std::vector<std::pair<wxString, double>>& data,
    const wxString& title,
    const wxString& xLabel,
    const wxString& yLabel) {
    chartData = data;
    chartTitle = title;
    xAxisLabel = xLabel;
    yAxisLabel = yLabel;
    hasData = true;

    // Refresh the panel to trigger repainting
    Refresh();
}

void ChartPanel::ClearChart() {
    chartData.clear();
    chartTitle = "";
    xAxisLabel = "";
    yAxisLabel = "";
    hasData = false;
    Refresh();
}

void ChartPanel::OnPaint(wxPaintEvent& event) {
    wxPaintDC dc(this);

    // If no data to display
    if (!hasData || chartData.empty()) {
        dc.SetTextForeground(*wxBLACK);
        dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
        wxString noDataText = "No data to display. Please select a data file.";
        wxSize textSize = dc.GetTextExtent(noDataText);
        dc.DrawText(noDataText,
            (GetSize().GetWidth() - textSize.GetWidth()) / 2,
            (GetSize().GetHeight() - textSize.GetHeight()) / 2);
        return;
    }

    // Chart dimensions and margins
    int margin = 60;
    int chartWidth = GetSize().GetWidth() - 2 * margin;
    int chartHeight = GetSize().GetHeight() - 2 * margin;
    int chartLeft = margin;
    int chartTop = margin;
    int chartBottom = chartTop + chartHeight;
    int chartRight = chartLeft + chartWidth;

    // Draw chart background and border
    dc.SetBrush(wxBrush(*wxWHITE));
    dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
    dc.DrawRectangle(chartLeft, chartTop, chartWidth, chartHeight);

    // Find min/max values
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();

    for (const auto& point : chartData) {
        minValue = std::min(minValue, point.second);
        maxValue = std::max(maxValue, point.second);
    }

    // Add some padding to the min/max values
    double valuePadding = (maxValue - minValue) * 0.1;
    if (valuePadding == 0) {  // If all values are the same
        valuePadding = std::abs(minValue) * 0.1;
        if (valuePadding == 0)  // If all values are 0
            valuePadding = 1.0;
    }

    minValue -= valuePadding;
    maxValue += valuePadding;

    // Draw title
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    wxSize titleSize = dc.GetTextExtent(chartTitle);
    dc.DrawText(chartTitle, chartLeft + (chartWidth - titleSize.GetWidth()) / 2, chartTop - 40);

    // Draw Y-axis label
    dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    dc.DrawRotatedText(yAxisLabel, chartLeft - 45, chartTop + chartHeight / 2 + dc.GetTextExtent(yAxisLabel).GetWidth() / 2, 90);

    // Draw X-axis label
    wxSize xLabelSize = dc.GetTextExtent(xAxisLabel);
    dc.DrawText(xAxisLabel, chartLeft + (chartWidth - xLabelSize.GetWidth()) / 2, chartBottom + 35);

    // Draw Y-axis values and grid lines
    dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    const int numYTicks = 5;
    for (int i = 0; i <= numYTicks; i++) {
        double value = minValue + ((maxValue - minValue) * i / numYTicks);
        int y = chartBottom - (i * chartHeight / numYTicks);

        // Format value based on its magnitude
        wxString valueStr;
        if (std::abs(value) < 0.01)
            valueStr = wxString::Format("%.4f", value);
        else if (std::abs(value) < 1)
            valueStr = wxString::Format("%.3f", value);
        else if (std::abs(value) < 10)
            valueStr = wxString::Format("%.2f", value);
        else if (std::abs(value) < 100)
            valueStr = wxString::Format("%.1f", value);
        else
            valueStr = wxString::Format("%.0f", value);

        wxSize textSize = dc.GetTextExtent(valueStr);
        dc.DrawText(valueStr, chartLeft - textSize.GetWidth() - 5, y - textSize.GetHeight() / 2);

        // Draw grid line
        wxPen gridPen(wxColour(220, 220, 220), 1, wxPENSTYLE_DOT);
        dc.SetPen(gridPen);
        dc.DrawLine(chartLeft, y, chartRight, y);
    }

    // Draw X-axis values (we'll show a subset of dates for readability)
    int numPointsToShow = std::min(10, static_cast<int>(chartData.size()));
    int step = std::max(1, static_cast<int>(chartData.size()) / numPointsToShow);

    for (size_t i = 0; i < chartData.size(); i += step) {
        wxString dateLabel = chartData[i].first;

        // If the date is in ISO format, try to make it more readable
        if (dateLabel.Length() > 10 && dateLabel.Contains('T')) {
            dateLabel = dateLabel.BeforeFirst('T');  // Take only the date part
        }

        int x = chartLeft + (i * chartWidth / (chartData.size() - 1));

        // Rotate the text for better readability
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawRotatedText(dateLabel, x - 5, chartBottom + 5, 45);

        // Draw vertical grid line
        wxPen gridPen(wxColour(220, 220, 220), 1, wxPENSTYLE_DOT);
        dc.SetPen(gridPen);
        dc.DrawLine(x, chartTop, x, chartBottom);
    }

    // Draw data points and connect them with lines
    if (chartData.size() > 1) {
        wxPen linePen(wxColour(41, 128, 185), 2);
        dc.SetPen(linePen);

        std::vector<wxPoint> points;

        for (size_t i = 0; i < chartData.size(); i++) {
            int x = chartLeft + (i * chartWidth / (chartData.size() - 1));
            int y = chartBottom - ((chartData[i].second - minValue) / (maxValue - minValue) * chartHeight);
            points.push_back(wxPoint(x, y));
        }

        // Draw polyline connecting all points
        dc.DrawLines(points.size(), points.data());

        // Draw data points
        for (const auto& point : points) {
            dc.SetBrush(wxBrush(wxColour(41, 128, 185)));
            dc.SetPen(wxPen(*wxWHITE, 1));
            dc.DrawCircle(point, 4);
        }
    }
}