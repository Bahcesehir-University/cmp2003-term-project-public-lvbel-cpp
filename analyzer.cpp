#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <iostream>

void TripAnalyzer::processCsvLine(const std::string& lineContent) {
    if (lineContent.empty()) return;

    size_t firstCommaIndex = lineContent.find(',');
    if (firstCommaIndex == std::string::npos) return;

    size_t secondCommaIndex = lineContent.find(',', firstCommaIndex + 1);
    if (secondCommaIndex == std::string::npos) return;

    if (secondCommaIndex <= firstCommaIndex + 1) return;
    std::string zoneIdentifier = lineContent.substr(firstCommaIndex + 1, secondCommaIndex - firstCommaIndex - 1);

    size_t timeSearchStartIndex = secondCommaIndex + 1;
    if (timeSearchStartIndex >= lineContent.size()) return;

    char firstCharAfterSecondComma = lineContent[timeSearchStartIndex];
    bool isStandardFormat = (firstCharAfterSecondComma >= '0' && firstCharAfterSecondComma <= '9');

    if (!isStandardFormat) {
        size_t thirdCommaIndex = lineContent.find(',', secondCommaIndex + 1);
        if (thirdCommaIndex == std::string::npos) return;
        timeSearchStartIndex = thirdCommaIndex + 1;
    }

    if (timeSearchStartIndex >= lineContent.size()) return;

    size_t spacePosition = lineContent.find(' ', timeSearchStartIndex);
    if (spacePosition == std::string::npos) return;

    if (spacePosition + 2 >= lineContent.size()) return;

    char firstHourDigit = lineContent[spacePosition + 1];
    char secondHourDigit = lineContent[spacePosition + 2];

    int pickupHour = 0;

    if (lineContent[spacePosition + 2] == ':') {
        if (firstHourDigit < '0' || firstHourDigit > '9') return;
        pickupHour = firstHourDigit - '0';
    } else {
        if (firstHourDigit < '0' || firstHourDigit > '9' || secondHourDigit < '0' || secondHourDigit > '9') return;
        pickupHour = (firstHourDigit - '0') * 10 + (secondHourDigit - '0');
    }

    if (pickupHour < 0 || pickupHour > 23) return;

    ZoneStatistics& statistics = zoneDataMap[zoneIdentifier];
    statistics.totalTrips++;
    statistics.tripsByHour[pickupHour]++;
}

void TripAnalyzer::ingestFile(const std::string& csvFilePath) {
    std::ifstream inputFileStream(csvFilePath);
    if (!inputFileStream.is_open()) return;

    std::string currentLine;
    if (!std::getline(inputFileStream, currentLine)) return;

    while (std::getline(inputFileStream, currentLine)) {
        processCsvLine(currentLine);
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> zoneResults;
    zoneResults.reserve(zoneDataMap.size());

    for (const auto& mapEntry : zoneDataMap) {
        zoneResults.push_back({mapEntry.first, mapEntry.second.totalTrips});
    }

    std::sort(zoneResults.begin(), zoneResults.end(), [](const ZoneCount& firstItem, const ZoneCount& secondItem) {
        if (firstItem.count != secondItem.count) return firstItem.count > secondItem.count;
        return firstItem.zone < secondItem.zone;
    });

    if (k >= 0 && (size_t)k < zoneResults.size()) {
        zoneResults.resize(k);
    }
    return zoneResults;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> slotResults;
    slotResults.reserve(zoneDataMap.size() * 5);

    for (const auto& mapEntry : zoneDataMap) {
        for (int hourIndex = 0; hourIndex < 24; ++hourIndex) {
            if (mapEntry.second.tripsByHour[hourIndex] > 0) {
                slotResults.push_back({mapEntry.first, hourIndex, mapEntry.second.tripsByHour[hourIndex]});
            }
        }
    }

    std::sort(slotResults.begin(), slotResults.end(), [](const SlotCount& firstItem, const SlotCount& secondItem) {
        if (firstItem.count != secondItem.count) return firstItem.count > secondItem.count;
        if (firstItem.zone != secondItem.zone) return firstItem.zone < secondItem.zone;
        return firstItem.hour < secondItem.hour;
    });

    if (k >= 0 && (size_t)k < slotResults.size()) {
        slotResults.resize(k);
    }
    return slotResults;
}
