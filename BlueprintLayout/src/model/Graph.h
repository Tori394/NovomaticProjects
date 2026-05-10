#pragma  once

#include <vector>
#include "Node.h"
#include "Edge.h"

namespace blueprint {
    class Graph {
        std::vector<Node> nodes;
        std::vector<Edge> edges;

    public:
        void setNodes(std::vector<Node> newNodes) { nodes = std::move(newNodes); }
        void setEdges(std::vector<Edge> newEdges) { edges = std::move(newEdges); }

        void addNode(const Node& node) {
            nodes.push_back(node);
        }

        void addEdge(int from, int to) {
            edges.push_back(Edge(from, to));
        }

        const std::vector<Node>& getNodes() const {
            return nodes;
        }

        const std::vector<Edge>& getEdges() const {
            return edges;
        }

        void updateNodePosition(int nodeId, float newX, float newY) {
            for (auto& node : nodes) {
                if (node.id == nodeId) {
                    node.x = newX;
                    node.y = newY;
                    break;
                }
            }
        }

        Node getNodeById(int id) const{
            for (const auto& node : nodes) {
                if (node.id == id) {
                    return node;
                }
            }
            return Node();
        }

        void updateNodeType(int nodeId, NodeType newType) {
            for (auto& node : nodes) {
                if (node.id == nodeId) {
                    node.type = newType;
                    break;
                }
            }
        }
    };
}
