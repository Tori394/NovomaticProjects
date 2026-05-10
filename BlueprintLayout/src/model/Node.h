#pragma once

#include <string>
#include <nlohmann/json.hpp>

namespace blueprint {
    enum class NodeType {
        ACTION = 0,
        BRANCH = 1,
        EVENT = 2
    };

    struct Node {
        int id;
        std::string name;
        float x;
        float y;
        NodeType type = NodeType::ACTION;

    };
    NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Node, id, name, x, y);
}