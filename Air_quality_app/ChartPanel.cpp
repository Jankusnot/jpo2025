/**
 * @file ChartPanel.cpp
 * @brief Implementation of the ChartPanel class for rendering data charts
 */

#include "ChartPanel.h"

 /**
  * @brief Constructor for the chart panel that initializes the panel with default settings
  * @param parent Pointer to the parent window
  *
  * Creates a new chart panel with a white background and binds the paint event handler.
  */
ChartPanel::ChartPanel(wxWindow* parent) : wxPanel(parent, wxID_ANY, wxDefaultPosition, wxSize(1200, 600)) {
    SetBackgroundColour(*wxWHITE);
    hasData = false;

    // Bind the paint event to handle rendering
    Bind(wxEVT_PAINT, &ChartPanel::OnPaint, this);
}

/**
 * @brief Sets chart data and properties for rendering and triggers a redraw
 * @param data Vector of timestamp-value pairs to be displayed
 * @param title Title of the chart
 * @param xLabel Label for the X axis
 * @param yLabel Label for the Y axis
 *
 * Updates the chart with new data and properties, then triggers a redraw to display the changes.
 */
void ChartPanel::SetChartData(const std::vector<std::pair<wxString, double>>& data, const wxString& title, const wxString& xLabel, const wxString& yLabel) {
    chartData = data;
    chartTitle = title;
    xAxisLabel = xLabel;
    yAxisLabel = yLabel;
    hasData = true;

    // Refresh the panel to trigger repainting
    Refresh();
}

/**
 * @brief Displays a message when no data is available
 * @param dc Device context for drawing
 *
 * Draws a centered message instructing the user to select a data file
 * when no data is available to display.
 */
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

/**
 * @brief Calculates chart dimensions and positioning based on panel size
 * @param chartLeft [out] Left edge position of chart area
 * @param chartTop [out] Top edge position of chart area
 * @param chartWidth [out] Width of chart area
 * @param chartHeight [out] Height of chart area
 * @param chartBottom [out] Bottom edge position of chart area
 * @param chartRight [out] Right edge position of chart area
 *
 * Sets up the chart dimensions with appropriate margins and calculates
 * all important coordinates for rendering the chart.
 */
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

/**
 * @brief Draws the chart background and border
 * @param dc Device context for drawing
 * @param chartLeft Left edge position of chart area
 * @param chartTop Top edge position of chart area
 * @param chartWidth Width of chart area
 * @param chartHeight Height of chart area
 *
 * Creates a white rectangle with a light gray border to serve as the chart background.
 */
void ChartPanel::DrawChartBackground(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight) {
    // Set background fill color to white
    dc.SetBrush(wxBrush(*wxWHITE));
    // Set border to light gray
    dc.SetPen(wxPen(wxColour(200, 200, 200), 1));
    // Draw the chart background rectangle
    dc.DrawRectangle(chartLeft, chartTop, chartWidth, chartHeight);
}

/**
 * @brief Determines the minimum and maximum data values with padding
 * @param minValue [out] Calculated minimum value for Y axis
 * @param maxValue [out] Calculated maximum value for Y axis
 *
 * Finds the minimum and maximum values in the data set and adds padding
 * to provide better visual display with appropriate spacing.
 */
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

/**
 * @brief Draws the chart title centered above the chart
 * @param dc Device context for drawing
 * @param chartLeft Left edge position of chart area
 * @param chartWidth Width of chart area
 * @param chartTop Top edge position of chart area
 *
 * Draws the chart title centered horizontally above the chart area.
 */
void ChartPanel::DrawChartTitle(wxPaintDC& dc, int chartLeft, int chartWidth, int chartTop) {
    dc.SetTextForeground(*wxBLACK);
    dc.SetFont(wxFont(12, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_BOLD));
    // Calculate title dimensions for centering
    wxSize titleSize = dc.GetTextExtent(chartTitle);
    // Draw title centered above chart
    dc.DrawText(chartTitle, chartLeft + (chartWidth - titleSize.GetWidth()) / 2, chartTop - 40);
}

/**
 * @brief Draws the X and Y axis labels
 * @param dc Device context for drawing
 * @param chartLeft Left edge position of chart area
 * @param chartTop Top edge position of chart area
 * @param chartWidth Width of chart area
 * @param chartHeight Height of chart area
 * @param chartBottom Bottom edge position of chart area
 *
 * Draws the X-axis label centered below the chart and the Y-axis label
 * rotated vertically and centered along the left side of the chart.
 */
void ChartPanel::DrawAxisLabels(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight, int chartBottom) {
    // Set font for axis labels
    dc.SetFont(wxFont(10, wxFONTFAMILY_DEFAULT, wxFONTSTYLE_NORMAL, wxFONTWEIGHT_NORMAL));
    // Draw Y-axis label vertically
    dc.DrawRotatedText(yAxisLabel, chartLeft - 45, chartTop + chartHeight / 2 + dc.GetTextExtent(yAxisLabel).GetWidth() / 2, 90);

    // Draw X-axis label horizontally
    wxSize xLabelSize = dc.GetTextExtent(xAxisLabel);
    dc.DrawText(xAxisLabel, chartLeft + (chartWidth - xLabelSize.GetWidth()) / 2, chartBottom + 35);
}

/**
 * @brief Draws Y-axis tick marks, labels, and horizontal grid lines
 * @param dc Device context for drawing
 * @param chartLeft Left edge position of chart area
 * @param chartRight Right edge position of chart area
 * @param chartBottom Bottom edge position of chart area
 * @param chartHeight Height of chart area
 * @param minValue Minimum value for Y axis
 * @param maxValue Maximum value for Y axis
 *
 * Creates evenly spaced tick marks along the Y-axis with appropriate labels
 * and dotted horizontal grid lines across the chart area.
 */
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

/**
 * @brief Draws X-axis tick marks, labels, and vertical grid lines
 * @param dc Device context for drawing
 * @param chartLeft Left edge position of chart area
 * @param chartTop Top edge position of chart area
 * @param chartBottom Bottom edge position of chart area
 * @param chartWidth Width of chart area
 *
 * Creates evenly spaced tick marks along the X-axis with timestamp labels
 * and dotted vertical grid lines across the chart area.
 */
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

/**
 * @brief Draws data points and the line connecting them
 * @param dc Device context for drawing
 * @param chartLeft Left edge position of chart area
 * @param chartBottom Bottom edge position of chart area
 * @param chartWidth Width of chart area
 * @param chartHeight Height of chart area
 * @param minValue Minimum value for Y axis
 * @param maxValue Maximum value for Y axis
 *
 * Creates a line graph by plotting data points as circles and connecting them
 * with a continuous line.
 */
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

/**
 * @brief Main paint handler that orchestrates the drawing of all chart elements
 * @param event The paint event
 *
 * Coordinates the drawing of all chart components in the correct order when
 * the panel needs to be repainted.
 */
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