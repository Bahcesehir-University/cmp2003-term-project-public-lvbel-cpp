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
private:
    struct ZoneStats {
        long long total = 0;
        long long hours[24] = {};
    };

    std::unordered_map<std::string, ZoneStats> data;
