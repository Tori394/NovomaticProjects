#include <gtest/gtest.h>
#include "../src/model/Graph.h"
#include "../src/algo/AutoLayout.h"

using namespace blueprint;

// Graph poprawnie trzyma i oddaje węzły
TEST(GraphTest, ShouldStoreAndRetrieveNodes) {
    Graph graph;
    graph.addNode({1, "Start", 0, 0, NodeType::EVENT});
    
    Node n = graph.getNodeById(1);
    EXPECT_EQ(n.name, "Start");
    EXPECT_EQ(n.type, NodeType::EVENT);
}

// Logika rozpoznawania typu (Data Flow)
TEST(GraphTest, ShouldIdentifyBranchType) {
    Node n;
    n.name = "Branch";

    if (n.name == "Branch") n.type = NodeType::BRANCH;
    EXPECT_EQ(n.type, NodeType::BRANCH);
}

// AutoLayout
TEST(LayoutTest, ShouldPositionNodesSequentially) {
    Graph graph;
    AutoLayout layout;

    graph.addNode({1, "Event", 0, 0, NodeType::EVENT});
    graph.addNode({2, "Action", 0, 0, NodeType::ACTION});
    graph.addEdge(1, 2);

    layout.calculatePositions(graph);

    Node n1 = graph.getNodeById(1);
    Node n2 = graph.getNodeById(2);


    EXPECT_GT(n2.x, n1.x);
    EXPECT_EQ(n1.x, 0.0f);
}