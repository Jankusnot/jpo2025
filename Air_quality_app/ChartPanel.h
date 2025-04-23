

#ifndef CHART_PANEL_H
#define CHART_PANEL_H

#include <wx/wx.h>
#include <algorithm>
#include <vector>
#include <utility>
#include <limits>

/**
 * @file ChartPanel.h
 * @brief Header file for the ChartPanel class that renders data charts
 */

 /**
  * @class ChartPanel
  * @brief A panel that displays time-series data as a line chart
  *
  * ChartPanel renders data points as a line chart with proper axes, labels, and grid.
  * It supports dynamic data updates and handles proper scaling of values.
  */
class ChartPanel : public wxPanel {
private:
    std::vector<std::pair<wxString, double>> chartData; /**< Data points to be displayed (timestamp, value) */
    wxString chartTitle; /**< Title displayed at the top of the chart */
    wxString xAxisLabel; /**< Label for the X axis */
    wxString yAxisLabel; /**< Label for the Y axis */
    bool hasData; /**< Flag indicating whether there is data to display */

    /**
     * @brief Event handler for paint events
     * @param event The paint event
     *
     * Coordinates the drawing of all chart elements when the panel needs to be repainted
     */
    void OnPaint(wxPaintEvent& event);

public:
    /**
     * @brief Constructor for ChartPanel
     * @param parent Parent window containing this panel
     *
     * Initializes a new chart panel with default settings
     */
    ChartPanel(wxWindow* parent);

    /**
     * @brief Sets the data to be displayed in the chart
     * @param data Vector of data points (timestamp, value pairs)
     * @param title Title for the chart
     * @param xLabel Label for the X axis
     * @param yLabel Label for the Y axis
     *
     * Updates the chart with new data and triggers a redraw
     */
    void SetChartData(const std::vector<std::pair<wxString, double>>& data, const wxString& title, const wxString& xLabel, const wxString& yLabel);

private:
    /**
     * @brief Displays a message when no data is available
     * @param dc Device context for drawing
     */
    void DrawNoDataMessage(wxPaintDC& dc);

    /**
     * @brief Calculates chart dimensions and positioning based on panel size
     * @param chartLeft [out] Left edge position of chart area
     * @param chartTop [out] Top edge position of chart area
     * @param chartWidth [out] Width of chart area
     * @param chartHeight [out] Height of chart area
     * @param chartBottom [out] Bottom edge position of chart area
     * @param chartRight [out] Right edge position of chart area
     */
    void SetupChartDimensions(int& chartLeft, int& chartTop, int& chartWidth, int& chartHeight, int& chartBottom, int& chartRight);

    /**
     * @brief Draws the chart background and border
     * @param dc Device context for drawing
     * @param chartLeft Left edge position of chart area
     * @param chartTop Top edge position of chart area
     * @param chartWidth Width of chart area
     * @param chartHeight Height of chart area
     */
    void DrawChartBackground(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight);

    /**
     * @brief Determines the minimum and maximum data values with padding
     * @param minValue [out] Calculated minimum value for Y axis
     * @param maxValue [out] Calculated maximum value for Y axis
     */
    void CalculateValueRange(double& minValue, double& maxValue);

    /**
     * @brief Draws the chart title centered above the chart
     * @param dc Device context for drawing
     * @param chartLeft Left edge position of chart area
     * @param chartWidth Width of chart area
     * @param chartTop Top edge position of chart area
     */
    void DrawChartTitle(wxPaintDC& dc, int chartLeft, int chartWidth, int chartTop);

    /**
     * @brief Draws the X and Y axis labels
     * @param dc Device context for drawing
     * @param chartLeft Left edge position of chart area
     * @param chartTop Top edge position of chart area
     * @param chartWidth Width of chart area
     * @param chartHeight Height of chart area
     * @param chartBottom Bottom edge position of chart area
     */
    void DrawAxisLabels(wxPaintDC& dc, int chartLeft, int chartTop, int chartWidth, int chartHeight, int chartBottom);

    /**
     * @brief Draws Y-axis tick marks, labels, and horizontal grid lines
     * @param dc Device context for drawing
     * @param chartLeft Left edge position of chart area
     * @param chartRight Right edge position of chart area
     * @param chartBottom Bottom edge position of chart area
     * @param chartHeight Height of chart area
     * @param minValue Minimum value for Y axis
     * @param maxValue Maximum value for Y axis
     */
    void DrawYAxisTicksAndGrid(wxPaintDC& dc, int chartLeft, int chartRight, int chartBottom, int chartHeight, double minValue, double maxValue);

    /**
     * @brief Draws X-axis tick marks, labels, and vertical grid lines
     * @param dc Device context for drawing
     * @param chartLeft Left edge position of chart area
     * @param chartTop Top edge position of chart area
     * @param chartBottom Bottom edge position of chart area
     * @param chartWidth Width of chart area
     */
    void DrawXAxisTicksAndGrid(wxPaintDC& dc, int chartLeft, int chartTop, int chartBottom, int chartWidth);

    /**
     * @brief Draws data points and the line connecting them
     * @param dc Device context for drawing
     * @param chartLeft Left edge position of chart area
     * @param chartBottom Bottom edge position of chart area
     * @param chartWidth Width of chart area
     * @param chartHeight Height of chart area
     * @param minValue Minimum value for Y axis
     * @param maxValue Maximum value for Y axis
     */
    void DrawDataPointsAndLines(wxPaintDC& dc, int chartLeft, int chartBottom, int chartWidth, int chartHeight, double minValue, double maxValue);
};

#endif // CHART_PANEL_H