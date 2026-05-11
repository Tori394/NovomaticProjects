#pragma once
#include <string>
#include <chrono>
#include <optional>

namespace analyzer {
    struct Query {
        std::optional<std::string> level;
        std::optional<std::string> source;
        std::optional<std::string> message;
        std::optional<std::chrono::system_clock::time_point> timestampFrom;
        std::optional<std::chrono::system_clock::time_point> timestampTo;
    };
}