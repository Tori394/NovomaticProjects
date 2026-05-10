#pragma once
#include  "../model/Graph.h"

namespace blueprint {
    class AutoLayout {
    public:
        static void calculatePositions(Graph& graph);
    };
}
