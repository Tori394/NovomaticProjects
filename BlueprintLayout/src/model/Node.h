#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace blueprint {
    struct Node {
        int id;
        std::string name;
        float x;
        float y;
        std::string type;

    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Node, id, name, x, y);
}