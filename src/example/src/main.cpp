#include <iostream>
#include <random>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <delaunay.hpp>


void draw_delaunay(delaunay::points_ref_t points) {
    for(const auto& point : points) {
        const int size = 2;
        DrawRectangle(point.x - size/2, point.y - size/2, size, size,  BLACK);
    }


    auto edges = delaunay::triangulate(points);

    for(const auto& edge : edges) {
        DrawLine(edge->start.x, edge->start.y, edge->end.x, edge->end.y, BLACK);
    }

    for(const auto& edge : edges) {
        if(edge->data == 1)
            DrawLine(edge->start.x, edge->start.y, edge->end.x, edge->end.y, GREEN);
    }

}

void draw_grid() {
    rlPushMatrix();
        rlTranslatef(200, 800, 0);
        rlRotatef(90, 1, 0, 0);
        DrawGrid(200, 10);
    rlPopMatrix();
}


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

int main() {
    delaunay::points_t points;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0, 200);
    std::uniform_real_distribution<float> velocity(-1.0f, 1.0f);

    for(int i = 0; i < 100; i++) {
        points.emplace_back(
            dist(rng), dist(rng)
        );
    }

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 800, "Delaunay Visualisation");

    while (!WindowShouldClose()) {
        update_camera(camera);

        for(int i = 0; i < 100; i++) {
            //points[i].x += velocity(rng) * 0.1f;
            //points[i].y += velocity(rng) * 0.1f;
        }

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                draw_grid();
                draw_delaunay(points);
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}