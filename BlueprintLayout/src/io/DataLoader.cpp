#include "DataLoader.h"
#include <fstream>

namespace blueprint {
    Graph DataLoader::loadGraph(const std::string& filePath) {
        std::ifstream file(filePath);
        if (!file.is_open()) {
            throw std::runtime_error("Nie udalo sie otworzyc pliku: " + filePath);
        }

        // Parse JSON
        nlohmann::json j;
        file >> j;

        Graph graph;

        // JSON -> vec
        auto nodes = j.at("nodes").get<std::vector<Node>>();
        graph.setEdges(j.at("edges").get<std::vector<Edge>>());

        for (auto& node : nodes) {
            if (node.name == "Branch") {
                node.type = NodeType::BRANCH;
            } else if (node.name.find("Event") != std::string::npos) {
                node.type = NodeType::EVENT;
            }
        }

        graph.setNodes(nodes);

        return graph;
    }
}