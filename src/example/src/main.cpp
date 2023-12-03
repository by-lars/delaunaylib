#include <iostream>
#include <random>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

struct Point {
    Point(float x, float y) : x(x), y(y) {}
    float x;
    float y;
};

void draw_delaunay(const std::vector<Point>& points) {
    for(const auto& point : points) {
        const int size = 8;
        DrawRectangle(point.x - size/2, point.y - size/2, size/2, size/2,  BLACK);
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
    if (IsMouseButtonDown(MOUSE_BUTTON_MIDDLE)) {
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
    std::vector<Point> points;
    std::random_device dev;
    std::mt19937 rng(dev());
    std::uniform_real_distribution<float> dist(0, 400);

    for(int i = 0; i < 100; i++) {
        points.emplace_back(
            dist(rng), dist(rng)
        );
    }

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(800, 450, "Delaunay Visualisation");

    while (!WindowShouldClose()) {
        update_camera(camera);

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