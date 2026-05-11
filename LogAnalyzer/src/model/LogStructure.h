#pragma once
#include <string>
#include <chrono>

namespace analyzer {
    struct LogStructure {
        std::chrono::system_clock::time_point timestamp;
        std::string level;
        std::string source;
        std::string message;
        std::string origin;
    };
}