#include <iostream>
#include <random>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <delaunay.hpp>

delaunay::points_t points;


void draw_delaunay(delaunay::points_ref_t points) {
    for(const auto& point : points) {
        const int size = 2;
        DrawRectangle(point.x - size/2, point.y - size/2, size, size,  BLACK);
    }


    auto edges = delaunay::triangulate(points);

    for(const auto& edge : edges) {
        if(edge.data == 1) {
           DrawLine(edge.origin.x, edge.origin.y, edge.destination.x, edge.destination.y, GREEN);
        } else if(edge.data == 2) {
           DrawLine(edge.origin.x, edge.origin.y, edge.destination.x, edge.destination.y, BLUE);
        } else if (edge.data == 3){
           DrawLine(edge.origin.x, edge.origin.y, edge.destination.x, edge.destination.y, RED);
        } else {
            DrawLine(edge.origin.x, edge.origin.y, edge.destination.x, edge.destination.y, ORANGE);
        }

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

    srand(420);
    for(int i = 0; i < 100; i++) {
        points.emplace_back(
            rand() % 500, rand() % 500
        );
    }

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;

    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1000, 800, "Delaunay Visualisation");

    while (!WindowShouldClose()) {
        update_camera(camera);

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 pos = GetScreenToWorld2D(GetMousePosition(), camera);
            points.emplace_back(pos.x, pos.y);
        }

        if(IsKeyPressed(KEY_D)) {
            points.clear();
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