#pragma once
#include <string>
#include <vector>
#include "../model/LogStructure.h"

namespace analyzer {
    class LogParser {
        public:
            LogStructure parseLog(std::string& log);
            std::vector<LogStructure> logs(const std::string& filename);
        };
}