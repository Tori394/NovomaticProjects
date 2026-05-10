#pragma once
#include  "../model/Graph.h"
#include <raylib.h>

namespace blueprint {
    class GraphRenderer {
        Camera2D camera;

        public:
            GraphRenderer(int screenWidth, int screenHeight);
            void updateCamera();
            void renderGraph(const Graph& graph);
            static void renderText();
    };
}