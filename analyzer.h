#pragma once
#include <string>
#include <vector>
#include <unordered_map>

struct ZoneCount {
    std::string zone;
    long long count;
};

struct SlotCount {
    std::string zone;
    int hour;
    long long count;
};

class TripAnalyzer {
public:
    void ingestFile(const std::string& csvFilePath);
    std::vector<ZoneCount> topZones(int k = 10) const;
    std::vector<SlotCount> topBusySlots(int k = 10) const;

private:
    struct ZoneStatistics {
        long long totalTrips = 0;
        long long tripsByHour[24] = {};
    };

    std::unordered_map<std::string, ZoneStatistics> zoneDataMap;
    void processCsvLine(const std::string& lineContent);
};
