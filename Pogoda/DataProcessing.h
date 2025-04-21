#ifndef DATA_PROCESSING_H
#define DATA_PROCESSING_H

#include <wx/wx.h>
#include <Json/json.h>
#include <algorithm>
#include <string>
#include <vector>

#include "InfoPanel.h"
#include "ChartPanel.h"

#include "NetworkingAndFileHandling.h"

void ProcessFileData(const std::string& filePath, InfoPanel* infoPanel, ChartPanel* chartPanel);
std::string CalculateTrend(const std::vector<std::pair<std::string, double>>& data);

#endif