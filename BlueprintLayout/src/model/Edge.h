#pragma once
#include <nlohmann/json.hpp>

namespace blueprint {
    struct Edge {
        int from;
        int to;
    };

    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Edge, from, to);
}