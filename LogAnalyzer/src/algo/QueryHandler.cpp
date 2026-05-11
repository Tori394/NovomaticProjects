#include "QueryHandler.h"

namespace analyzer {
    std::vector<LogStructure> QueryHandler::execute(const std::vector<LogStructure>& logs, const Query& query) {
        std::vector<LogStructure> results;

        for (const auto& entry : logs) {
            if (query.level && entry.level != *query.level)
                continue;

            if (query.source && entry.source != *query.source)
                continue;

            if (query.message && entry.message.find(*query.message) == std::string::npos)
                continue;

            if (query.timestampFrom && entry.timestamp < *query.timestampFrom)
                continue;

            if (query.timestampTo && entry.timestamp > *query.timestampTo)
                continue;

            results.push_back(entry);
        }

        return results;
    }
}