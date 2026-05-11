#pragma once
#include "../model/LogStructure.h"
#include "../model/Query.h"
#include <vector>

namespace analyzer {
    class QueryHandler {
    public:
        std::vector<LogStructure> execute(const std::vector<LogStructure>& logs, const Query& query);
    };
}