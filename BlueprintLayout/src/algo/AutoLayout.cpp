#include "AutoLayout.h"
#include <map>
#include <vector>

namespace blueprint {
    void AutoLayout::calculatePositions(Graph& graph) {
        auto nodes = graph.getNodes();
        auto edges = graph.getEdges();
        if (nodes.empty()) return;

        // Najadłuższa scieżka dla kazdego wezla
        std::map<int, int> nodeDepth; // id wezla -> numer kolumny (X)
        for (const auto& node : nodes) {
            nodeDepth[node.id] = 0; // Na start wszystkie są w 0
        }

        for (size_t i = 0; i < nodes.size(); ++i) {
            for (const auto& edge : edges) {
                if (nodeDepth[edge.to] < nodeDepth[edge.from] + 1) {
                    nodeDepth[edge.to] = nodeDepth[edge.from] + 1;
                }
            }
        }

        // Grupowanie po kolumnach
        std::map<int, std::vector<int>> columns; // numer kolumny -> lista id węzłów
        for (const auto& node : nodes) {
            columns[nodeDepth[node.id]].push_back(node.id);
        }


        for (const auto& [columnIndex, nodesInColumn] : columns) {
            const float spaceY = 150.0f;
            const float spaceX = 250.0f;

            // x calej kolumny
            float xPos = columnIndex * spaceX;

            // wyśrodkowany y
            int count = nodesInColumn.size();
            float startY = -((count - 1) * spaceY) / 2.0f;

            // wezly w kolumnie
            for (int i = 0; i < count; ++i) {
                int nodeId = nodesInColumn[i];
                float yPos = startY + (i * spaceY);

                // aktualizacja pozycji
                graph.updateNodePosition(nodeId, xPos, yPos);
            }
        }
    }
}