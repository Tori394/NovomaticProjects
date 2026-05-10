#include "io/DataLoader.h"
#include "view/GraphRenderer.h"
#include <iostream>
#include <raylib.h>

#include "algo/AutoLayout.h"

int main() {
    blueprint::Graph myGraph;

    try {
        myGraph = blueprint::DataLoader::loadGraph("data/blueprint.json");
    } catch (const std::exception& e) {
        std::cerr << "Blad wczytywania: " << e.what() << std::endl;
        return 1;
    }

    const int screenWidth = 1024;
    const int screenHeight = 768;
    InitWindow(screenWidth, screenHeight, "Blueprint Auto Layout");
    SetTargetFPS(60);

    blueprint::AutoLayout::calculatePositions(myGraph);
    blueprint::GraphRenderer renderer(screenWidth, screenHeight);

    while (!WindowShouldClose()) {
        renderer.updateCamera();
        renderer.renderGraph(myGraph);

        EndMode2D();
        renderer.renderText();

        EndDrawing();
    }

    CloseWindow();
    return 0;
}
