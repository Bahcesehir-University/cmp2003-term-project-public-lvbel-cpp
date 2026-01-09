#include "analyzer.h"

void TripAnalyzer::processLine(const std::string& line) {
    if (line.empty()) return;

    size_t comma1 = line.find(',');
    if (comma1 == std::string::npos) return;
    size_t comma2 = line.find(',', comma1 + 1);
    if (comma2 == std::string::npos) return;
    size_t comma3 = line.find(',', comma2 + 1);
    if (comma3 == std::string::npos) return;
    size_t comma4 = line.find(',', comma3 + 1);
    if (comma4 == std::string::npos) return;
    size_t comma5 = line.find(',', comma4 + 1);
    if (comma5 == std::string::npos) return;

    if (comma2 - comma1 <= 1) return;
    if (comma4 - comma3 <= 1) return;

    size_t searchStart = comma3 + 1;
    size_t searchEnd = comma4;
    size_t spacePos = line.find(' ', searchStart);

    if (spacePos == std::string::npos || spacePos >= searchEnd) return;
    if (spacePos + 2 >= searchEnd) return;

    char hourDigit1 = line[spacePos + 1];
    char hourDigit2 = line[spacePos + 2];

    if (hourDigit1 < '0' || hourDigit1 > '9' || hourDigit2 < '0' || hourDigit2 > '9') return;

    int pickupHour = (hourDigit1 - '0') * 10 + (hourDigit2 - '0');
    if (pickupHour > 23) return;

    std::string pickupZoneID = line.substr(comma1 + 1, comma2 - comma1 - 1);
    
    ZoneStats& stats = data[pickupZoneID];
    stats.total++;
    stats.hours[pickupHour]++;
}

void TripAnalyzer::ingestStdin() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    std::string line;
    if (!std::getline(std::cin, line)) return;
    
    while (std::getline(std::cin, line)) {
        processLine(line);
    }
}

void TripAnalyzer::ingestFile(const std::string& path) {
    std::ifstream file(path);
    if (!file.is_open()) return;

    std::string line;
    if (!std::getline(file, line)) return;

    while (std::getline(file, line)) {
        processLine(line);
    }
}

std::vector<ZoneCount> TripAnalyzer::topZones(int k) {
    std::vector<ZoneCount> result;
    result.reserve(data.size());
    for (const auto& pair : data) {
        result.push_back({pair.first, pair.second.total});
    }

    auto comparator = [](const ZoneCount& a, const ZoneCount& b) {
        if (a.count != b.count) return a.count > b.count;
        return a.zone < b.zone;
    };

    if (k < 0 || (size_t)k >= result.size()) {
        std::sort(result.begin(), result.end(), comparator);
    } else {
        std::partial_sort(result.begin(), result.begin() + k, result.end(), comparator);
        result.resize(k);
    }
    return result;
}

std::vector<SlotCount> TripAnalyzer::topBusySlots(int k) {
    std::vector<SlotCount> result;
    for (const auto& pair : data) {
        for (int h = 0; h < 24; ++h) {
            if (pair.second.hours[h] > 0) {
                result.push_back({pair.first, h, pair.second.hours[h]});
            }
        }
    }

    auto comparator = [](const SlotCount& a, const SlotCount& b) {
        if (a.count != b.count) return a.count > b.count;
        if (a.zone != b.zone) return a.zone < b.zone;
        return a.hour < b.hour;
    };

    if (k < 0 || (size_t)k >= result.size()) {
        std::sort(result.begin(), result.end(), comparator);
    } else {
        std::partial_sort(result.begin(), result.begin() + k, result.end(), comparator);
        result.resize(k);
    }
    return result;
}
