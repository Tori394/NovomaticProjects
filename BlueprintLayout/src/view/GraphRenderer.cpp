#include "GraphRenderer.h"
#include <raylib.h>
#include <raymath.h>

namespace blueprint {
    GraphRenderer::GraphRenderer(int screenWidth, int screenHeight) {
        camera = { 0 };
        camera.target = Vector2{ 0.0f, 0.0f };
        camera.offset = Vector2{ screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    }

    void GraphRenderer::updateCamera() {
        if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
            Vector2 delta = GetMouseDelta();
            delta = Vector2Scale(delta, -1.0f / camera.zoom);
            camera.target = Vector2Add(camera.target, delta);
        }

        float wheel = GetMouseWheelMove();
        if (wheel != 0) {
            Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);
            camera.offset = GetMousePosition();
            camera.target = mouseWorldPos;

            camera.zoom += wheel * 0.125f;
            if (camera.zoom < 0.5f) camera.zoom = 0.5f;
            if (camera.zoom > 2.0f) camera.zoom = 2.0f;
        }
    }

    void GraphRenderer::renderGraph(const Graph &graph) {
        BeginDrawing();
        ClearBackground(RAYWHITE);

        BeginMode2D(camera);

        for (const auto& edge : graph.getEdges()) {
            auto nodeA = graph.getNodeById(edge.from);
            auto nodeB = graph.getNodeById(edge.to);

            DrawLineEx(
                Vector2{nodeA.x, nodeA.y},
                Vector2{nodeB.x, nodeB.y},
                2.0f, DARKGRAY
            );
        }

        for (const auto& node : graph.getNodes()) {
            switch (node.type) {
                case NodeType::BRANCH:
                    // Romb
                    DrawPoly(Vector2{node.x, node.y}, 4, 30.0f, 0.0f, ORANGE);
                    break;

                case NodeType::EVENT:
                    // Duże kółko
                    DrawCircle(node.x, node.y, 30, GREEN);
                    DrawText(node.name.c_str(), node.x - 25, node.y - 45, 18, BLACK);
                    break;

                default:
                    // Wszystkie inne typy
                    DrawCircle(node.x, node.y, 20, BLUE);
                    DrawText(node.name.c_str(), node.x - 25, node.y - 35, 18, BLACK);
                    break;
            }
        }
    }

    void GraphRenderer::renderText() {
        DrawText("Lewy przycisk myszy - przewijanie ekranu", 10, 10, 10, BLACK);
        DrawText("Scroll - przyblizanie", 10, 35, 10, BLACK);
    }

}