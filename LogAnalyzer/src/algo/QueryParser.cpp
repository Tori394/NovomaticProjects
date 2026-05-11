#include "QueryParser.h"
#include <regex>
#include <sstream>
#include <iostream>

namespace analyzer {
    std::chrono::system_clock::time_point QueryParser::parseTimestamp(const std::string& s) {
        std::istringstream ss(s);
        std::tm tm{};
        ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
        return std::chrono::system_clock::from_time_t(std::mktime(&tm));
    }

    Query QueryParser::parse(const std::string& line) {
        Query query;

        static const std::regex tokenPattern(R"REGEX((\w+)="([^"]+)")REGEX");
        std::sregex_iterator begin(line.begin(), line.end(), tokenPattern);
        std::sregex_iterator end;

        for (auto it = begin; it != end; ++it) {
            std::string key   = (*it)[1].str();
            std::string value = (*it)[2].str();

            if (key == "LOG_LEVEL") query.level = value;
            else if (key == "SOURCE") query.source = value;
            else if (key == "MESSAGE") query.message = value;
            else if (key == "TIMESTAMP_FROM") query.timestampFrom = parseTimestamp(value);
            else if (key == "TIMESTAMP_TO") query.timestampTo = parseTimestamp(value);
            else
                std::cerr << "Nieznany klucz: " << key << "\n";
        }

        return query;
    }
}