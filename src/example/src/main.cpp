#include <iostream>
#include <random>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include "delaunay/delaunay.hpp"

void update_camera(Camera2D& camera) {
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT)) {
        Vector2 delta = GetMouseDelta();
        delta = Vector2Scale(delta, -1.0f/camera.zoom);

        camera.target = Vector2Add(camera.target, delta);
    }

    // Zoom based on mouse wheel
    float wheel = GetMouseWheelMove();
    if (wheel != 0) {
        // Get the world point that is under the mouse
        Vector2 mouseWorldPos = GetScreenToWorld2D(GetMousePosition(), camera);

        // Set the offset to where the mouse is
        camera.offset = GetMousePosition();

        // Set the target to match, so that the camera maps the world space point
        // under the cursor to the screen space point under the cursor at any zoom
        camera.target = mouseWorldPos;

        // Zoom increment
        const float zoomIncrement = 0.125f;


        camera.zoom += (wheel*zoomIncrement);
        if (camera.zoom < zoomIncrement) camera.zoom = zoomIncrement;
    }
}

auto gen_random_points(size_t num_points) -> std::vector<delaunay::point_t> {
    std::vector<delaunay::point_t> points;
    std::uniform_real_distribution<double> distr(-500, 500);
    std::default_random_engine rand;

    for (size_t i = 0; i < num_points; i++) {
        const double x = distr(rand);
        const double y = distr(rand);
        points.emplace_back(x, y);
    }

    return points;
}

void draw_edges(const std::vector<delaunay::QuadEdge*>& edges, Color color) {
    for(auto* edge : edges) {
        if(edge->is_deleted()) {
            continue;
        }

        DrawLine(static_cast<int>(edge->origin().x),
                 static_cast<int>(edge->origin().y),
                 static_cast<int>(edge->destination().x),
                 static_cast<int>(edge->destination().y),
                 color);

        DrawCircle(static_cast<int>(edge->origin().x),
                   static_cast<int>(edge->origin().y),1.0, color);
    }
}

int main() {
    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1000, 800, "Delaunay Visualisation");

    auto points = gen_random_points(1000);

    while (!WindowShouldClose()) {
        update_camera(camera);

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 pos = GetScreenToWorld2D(GetMousePosition(), camera);
            points.emplace_back(pos.x, pos.y);
        }

        delaunay::Delaunay triangulation = delaunay::Delaunay::triangulate(points);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);

                draw_edges(triangulation.get_primary_edges(), BLACK);
                draw_edges(triangulation.get_dual_edges(), ORANGE);


            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}