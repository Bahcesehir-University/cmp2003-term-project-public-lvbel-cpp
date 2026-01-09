#include "analyzer.h"
#include <fstream>
#include <algorithm>
#include <iostream>

void TripAnalyzer::ingestFile(const std::string& csvPath) {
    std::ifstream file(csvPath);
    if (!file.is_open()) return;

    std::string line;
    if (!std::getline(file, line)) return;

    while (std::getline(file, line)) {
        if (line.empty()) continue;

size_t c1 = line.find(',');
if (c1 == std::string::npos) continue;
size_t c2 = line.find(',', c1 + 1);
if (c2 == std::string::npos) continue;
if (c2 <= c1 + 1) continue;
std::string zone = line.substr(c1 + 1, c2 - c1 - 1);
size_t timeStart = c2 + 1;
if (timeStart >= line.size()) continue;
size_t spacePos = line.find(' ', timeStart);
if (spacePos == std::string::npos) continue;
size_t hourPos = spacePos + 1;
if (hourPos >= line.size()) continue;
size_t colonPos = line.find(':', hourPos);
if (colonPos == std::string::npos) continue;
std::string hourStr = line.substr(hourPos, colonPos - hourPos);
if (hourStr.empty()) continue;
    int hour = 0;
    try {
            hour = std::stoi(hourStr);
        } catch (...) {
            continue;
        }

        if (hour < 0 || hour > 23) continue;

        m_zoneCounts[zone]++;
        m_zoneHourlyCounts[zone].hours[hour]++;
    }
}
std::vector<ZoneCount> TripAnalyzer::topZones(int k) const {
    std::vector<ZoneCount> results;
    results.reserve(m_zoneCounts.size());

    for (const auto& kv : m_zoneCounts) {
        results.push_back({kv.first, kv.second});
    }
    std::sort(results.begin(), results.end(), [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        return a.zone < b.zone;
    });

    if ((int)results.size() > k) {
        results.resize(k);
    }
    return results;
}
std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) const {
    std::vector<SlotCount> results;
    results.reserve(m_zoneHourlyCounts.size() * 5);

    for (const auto& kv : m_zoneHourlyCounts) {
        const std::string& zone = kv.first;
        const auto& tracker = kv.second;

        for (int h = 0; h < 24; h++) {
            if (tracker.hours[h] > 0) {
                results.push_back({zone, h, tracker.hours[h]});
            }
        }
    }
    std::sort(results.begin(), results.end(), [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) {
            return a.count > b.count;
        }
        if (a.zone != b.zone) {
            return a.zone < b.zone;
        }
        return a.hour < b.hour;
    });
    if ((int)results.size() > k) {
        results.resize(k);
    }
    return results;
}
