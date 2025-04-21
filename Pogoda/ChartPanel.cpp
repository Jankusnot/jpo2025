#include "ChartPanel.h"

// Constructor for the chart panel that initializes the panel with default settings
ChartPanel::ChartPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1200, 600)) {
    SetBackgroundColour(*wxWHITE);
    hasData = false;

    // Bind the paint event to handle rendering
    Bind(wxEVT_PAINT, &ChartPanel::OnPaint, this);
}

// Sets chart data and properties for rendering and triggers a redraw
void ChartPanel::SetChartData(const std::vector<std::pair<wxString, double>>& data, const wxString& title, const wxString& xLabel, const wxString& yLabel) {
    chartData = data;
    chartTitle = title;
    xAxisLabel = xLabel;
    yAxisLabel = yLabel;
    hasData = true;

    // Refresh the panel to trigger repainting
    Refresh();
}

// Displays a message when no data is available
void ChartPanel::DrawNoDataMessage(wxPaintDC& dc) {
    // Set text properties for the message
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    wxString noDataText = "No data to display. Please select a data file.";
    // Calculate text dimensions for centering
    wxSize textSize = dc.GetTextExtent(noDataText);
    // Draw the message centered in the panel
    dc.DrawText(noDataText,
        (GetSize().GetWidth() - textSize.GetWidth()) / 2,
        (GetSize().GetHeight() - textSize.GetHeight()) / 2);
}

// Calculates chart dimensions and positioning based on panel size
void ChartPanel::SetupChartDimensions(int& chartLeft, int& chartTop, int& chartWidth, int& chartHeight, int& chartBottom, int& chartRight) {
    int margin = 60;
    // Calculate chart dimensions with margins
    chartWidth = GetSize().GetWidth() - 2 * margin;
    chartHeight = GetSize().GetHeight() - 2 * margin;
    // Calculate chart position
    chartLeft = margin;
    chartTop = margin;
    // Calculate bottom and right edges
    chartBottom = chartTop + chartHeight;
    chartRight = chartLeft + chartWidth;
}

// Draws the chart background and border
void ChartPanel::DrawChartBackground(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight) {
    // Set background fill color to white
    dc.SetBrush(wxBrush(*wxWHITE));
    // Set border to light gray
    dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
    // Draw the chart background rectangle
    dc.DrawRectangle(chartLeft, chartTop, chartWidth, chartHeight);
}

// Determines the minimum and maximum data values with padding
void ChartPanel::CalculateValueRange(double& minValue, double& maxValue) {
    // Initialize min/max to extreme values
    minValue = std::numeric_limits<double>::max();
    maxValue = std::numeric_limits<double>::lowest();

    // Find actual min/max values in data
    for (const auto& point : chartData) {
        minValue = std::min(minValue, point.second);
        maxValue = std::max(maxValue, point.second);
    }

    // Add padding to min/max values for better visual display
    double valuePadding = (maxValue - minValue) * 0.1;
    if (valuePadding == 0) {
        valuePadding = std::abs(minValue) * 0.1;
        if (valuePadding == 0)
            valuePadding = 1.0;
    }

    // Apply padding to range
    minValue -= valuePadding;
    maxValue += valuePadding;
}

// Draws the chart title centered above the chart
void ChartPanel::DrawChartTitle(wxPaintDC& dc, int chartLeft, int chartWidth, int chartTop) {
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    // Calculate title dimensions for centering
    wxSize titleSize = dc.GetTextExtent(chartTitle);
    // Draw title centered above chart
    dc.DrawText(chartTitle, chartLeft + (chartWidth - titleSize.GetWidth()) / 2, chartTop - 40);
}

// Draws the X and Y axis labels
void ChartPanel::DrawAxisLabels(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight, int chartBottom) {
    // Set font for axis labels
    dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    // Draw Y-axis label vertically
    dc.DrawRotatedText(yAxisLabel, chartLeft - 45, chartTop + chartHeight / 2 + dc.GetTextExtent(yAxisLabel).GetWidth() / 2, 90);

    // Draw X-axis label horizontally
    wxSize xLabelSize = dc.GetTextExtent(xAxisLabel);
    dc.DrawText(xAxisLabel, chartLeft + (chartWidth - xLabelSize.GetWidth()) / 2, chartBottom + 35);
}

// Draws Y-axis tick marks, labels, and horizontal grid lines
void ChartPanel::DrawYAxisTicksAndGrid(wxPaintDC& dc, int chartLeft, int chartRight, int chartBottom, int chartHeight, double minValue, double maxValue) {
    dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));

    // Define number of Y-axis tick marks
    const int numYTicks = 5;
    for (int i = 0; i <= numYTicks; i++) {
        // Calculate value for this tick
        double value = minValue + ((maxValue - minValue) * i / numYTicks);
        // Calculate Y position for this tick
        int y = chartBottom - (i * chartHeight / numYTicks);

        // Format value based on its magnitude for better readability
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

        // Calculate label width for alignment
        wxSize textSize = dc.GetTextExtent(valueStr);
        // Draw the tick label
        dc.DrawText(valueStr, chartLeft - textSize.GetWidth() - 5, y - textSize.GetHeight() / 2);

        // Draw horizontal grid line
        wxPen gridPen(wxColour(220, 220, 220), 1, wxPENSTYLE_DOT);
        dc.SetPen(gridPen);
        dc.DrawLine(chartLeft, y, chartRight, y);
    }
}

// Draws X-axis tick marks, labels, and vertical grid lines
void ChartPanel::DrawXAxisTicksAndGrid(wxPaintDC& dc, int chartLeft, int chartTop, int chartBottom, int chartWidth) {
    // Determine how many X-axis labels to display
    int numPointsToShow = std::min(10, static_cast<int>(chartData.size()));
    int step = std::max(1, static_cast<int>(chartData.size()) / numPointsToShow);

    // Draw labels at regular intervals
    for (size_t i = 0; i < chartData.size(); i += step) {
        wxString dateLabel = chartData[i].first;

        // Format date string to "d00 h00" format
        dateLabel = "d" + dateLabel.substr(8, 2) + " h" + dateLabel.substr(11, 2);

        // Calculate X position for this label
        int x = chartLeft + (i * chartWidth / (chartData.size() - 1));

        // Draw rotated label for better space utilization
        dc.SetFont(wxFont(8, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
        dc.DrawRotatedText(dateLabel, x - 5, chartBottom + 5, 45);

        // Draw vertical grid line
        wxPen gridPen(wxColour(220, 220, 220), 1, wxPENSTYLE_DOT);
        dc.SetPen(gridPen);
        dc.DrawLine(x, chartTop, x, chartBottom);
    }
}

// Draws data points and the line connecting them
void ChartPanel::DrawDataPointsAndLines(wxPaintDC& dc, int chartLeft, int chartBottom, int chartWidth, int chartHeight, double minValue, double maxValue) {
    if (chartData.size() > 1) {
        wxPen linePen(wxColour(41, 128, 185), 2);
        dc.SetPen(linePen);

        std::vector<wxPoint> points;

        // Calculate screen coordinates for each data point
        for (size_t i = 0; i < chartData.size(); i++) {
            int x = chartLeft + (i * chartWidth / (chartData.size() - 1));
            // Map data value to screen Y coordinate
            int y = chartBottom - ((chartData[i].second - minValue) / (maxValue - minValue) * chartHeight);
            points.push_back(wxPoint(x, y));
        }

        // Draw polyline connecting all points
        dc.DrawLines(points.size(), points.data());

        // Draw each data point as a circle
        for (const auto& point : points) {
            dc.SetBrush(wxBrush(wxColour(41, 128, 185)));
            dc.SetPen(wxPen(*wxWHITE, 1));
            dc.DrawCircle(point, 4);
        }
    }
}

// Main paint handler that orchestrates the drawing of all chart elements
void ChartPanel::OnPaint(wxPaintEvent& event) {
    // Create device context for drawing
    wxPaintDC dc(this);

    // Show message if no data is available
    if (!hasData || chartData.empty()) {
        DrawNoDataMessage(dc);
        return;
    }

    // Set up chart dimensions
    int chartLeft, chartTop, chartWidth, chartHeight, chartBottom, chartRight;
    SetupChartDimensions(chartLeft, chartTop, chartWidth, chartHeight, chartBottom, chartRight);

    // Draw chart background and border
    DrawChartBackground(dc, chartLeft, chartTop, chartWidth, chartHeight);

    // Calculate data value range
    double minValue, maxValue;
    CalculateValueRange(minValue, maxValue);

    // Draw all chart elements in proper order
    DrawChartTitle(dc, chartLeft, chartWidth, chartTop);
    DrawAxisLabels(dc, chartLeft, chartTop, chartWidth, chartHeight, chartBottom);
    DrawYAxisTicksAndGrid(dc, chartLeft, chartRight, chartBottom, chartHeight, minValue, maxValue);
    DrawXAxisTicksAndGrid(dc, chartLeft, chartTop, chartBottom, chartWidth);
    DrawDataPointsAndLines(dc, chartLeft, chartBottom, chartWidth, chartHeight, minValue, maxValue);
}