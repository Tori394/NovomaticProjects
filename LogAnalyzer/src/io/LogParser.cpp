#include "LogParser.h"
#include <fstream>
#include <regex>
#include <sstream>

using namespace std;

namespace analyzer {
        LogStructure LogParser::parseLog(string& log) {
                // [TIMESTAMP] [LEVEL] [SOURCE] Message
                static const std::regex pattern(
                    R"(\[(\S+)\] \[(\w+)\] \[(\w+)\] (.+))"
                );
                smatch match;
                if (!regex_match(log,match,pattern)) {
                        throw runtime_error("Niepoprawna linia: " + log);
                }

                LogStructure parsedLog;
                parsedLog.origin = log;
                istringstream ss(match[1].str());
                tm tm{};
                ss >> get_time(&tm, "%Y-%m-%dT%H:%M:%S");
                parsedLog.timestamp = chrono::system_clock::from_time_t(mktime(&tm));
                parsedLog.level = match[2].str();
                parsedLog.source = match[3].str();
                parsedLog.message = match[4].str();

                return parsedLog;
        }

        vector<LogStructure> LogParser::logs(const string& filename) {
                ifstream file(filename);
                vector<LogStructure> logs;
                if (file.is_open()) {
                        string line;
                        while (getline(file, line)) {
                                if (line.empty()) continue;
                                logs.push_back(parseLog(line));
                        }
                        file.close();
                }
                return logs;
        }
}