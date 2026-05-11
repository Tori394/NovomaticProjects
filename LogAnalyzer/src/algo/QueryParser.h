#pragma once
#include "../model/Query.h"
#include <string>
#include <chrono>

namespace analyzer {
    class QueryParser {
    public:
        Query parse(const std::string& line);
    private:
        std::chrono::system_clock::time_point parseTimestamp(const std::string& s);
    };
}