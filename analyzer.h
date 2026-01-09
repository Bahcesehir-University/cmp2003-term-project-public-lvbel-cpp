#pragma once
#include <string>
#include <vector>
#include <unordered_map>
#include <iostream>
#include <fstream>
#include <algorithm>

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
private:
    struct ZoneStats {
        long long total = 0;
        long long hours[24] = {};
    };

    std::unordered_map<std::string, ZoneStats> data;

    void processLine(const std::string& line);

public:
    void ingestStdin();
    void ingestFile(const std::string& path);

    std::vector<ZoneCount> topZones(int k = 10);
    std::vector<SlotCount> topBusySlots(int k = 10);
};
