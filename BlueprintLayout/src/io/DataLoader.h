#pragma once
#include "../model/Graph.h"
#include <string>

namespace blueprint {
    class DataLoader {
    public:
        static Graph loadGraph(const std::string& filePath);
    };
}