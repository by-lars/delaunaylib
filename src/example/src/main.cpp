#include <iostream>
#include <random>
#include <vector>
#include <raylib.h>
#include <raymath.h>
#include <rlgl.h>

#include <delaunay.hpp>

void draw_delaunay(const std::vector<delaunay::Edge>& edges) {
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

void draw_points(delaunay::points_ref_t points, Color color, float size) {
    for(const auto& point : points) {
        DrawRectangle(point.x - size/2.0f, point.y - size/2.0f, size, size, color);
    }
}

Vector2 to_vec2(delaunay::point_ref_t p) {
    return {p.x,p.y};
}

void draw_triangles(const std::vector<delaunay::Triangle>& triangles) {
    srand(0);
    for(const auto& t : triangles) {
        Color c {uint8_t(rand() % 255), uint8_t(rand() % 255), uint8_t(rand() % 255), 255};
        DrawTriangle(to_vec2(t.a->origin()), to_vec2(t.b->origin()), to_vec2(t.c->origin()), c);
    }
}

void draw_grid() {
    rlPushMatrix();
        rlTranslatef(200, 800, 0);
        rlRotatef(90, 1, 0, 0);
        DrawGrid(200, 10);
    rlPopMatrix();
}


void draw_car(delaunay::point_ref_t pos, float theta) {
    rlPushMatrix();
        rlTranslatef(pos.x, pos.y, 0.0f);
            rlRotatef(theta, 0.0f, 0.0f, 1.0f);
            DrawLine(0, 0, 20, 0, RED);
            DrawLine(0, 0, 0, 20, BLUE);
        rlTranslatef(-pos.x - 10, -pos.y - 5, 0.0f);

        DrawRectangle(pos.x, pos.y, 20, 10, BLACK);

        DrawRectangle(pos.x+2, pos.y+10, 4, 2, GRAY);
        DrawRectangle(pos.x+12, pos.y+10, 4, 2, GRAY);
        DrawRectangle(pos.x+2, pos.y-2, 4, 2, GRAY);
        DrawRectangle(pos.x+12, pos.y-2, 4, 2, GRAY);
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

delaunay::points_t gen_race_track_points() {
    delaunay::points_t points;

    const int num_points = 100;
    const int radius = 500;
    const int track_width = 50;

    float alpha = 0.0f;
    for (int i = 0; i < num_points; i++) {

        int off = rand() % 10;


        float x = (radius - off) * cos(alpha);
        float y = (radius - off) * sin(alpha);
        points.emplace_back(x, y);

        x = (radius - track_width - off) * cos(alpha);
        y = (radius - track_width - off) * sin(alpha);
        points.emplace_back(x, y);

        alpha += 360.0f / (float)num_points;
    }

    return points;
}

delaunay::points_t gen_middle_points(const std::vector<delaunay::Edge>& edges) {
    delaunay::points_t points;

    for(const auto& edge : edges) {
        points.emplace_back((edge.origin.x + edge.destination.x) / 2.0f, (edge.origin.y + edge.destination.y) / 2.0f);
    }

    return points;
}

float dist_squared(delaunay::point_ref_t a, delaunay::point_ref_t b) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return dx*dx + dy*dy;
}

delaunay::points_t find_trajectory(delaunay::point_ref_t pos, delaunay::points_t points) {
    delaunay::points_t target_points;

    std::sort(points.begin(), points.end(), [&](delaunay::point_ref_t a, delaunay::point_ref_t b) {
        float dist_a = dist_squared(a, pos);
        float dist_b = dist_squared(b, pos);
        return dist_a < dist_b;
    });

    target_points.push_back(points[0]);

    return target_points;
}

int main() {
    delaunay::points_t points = gen_race_track_points();

    Camera2D camera = { 0 };
    camera.zoom = 1.0f;
    SetConfigFlags(FLAG_MSAA_4X_HINT);
    InitWindow(1000, 800, "Delaunay Visualisation");

    delaunay::point_t car_position(0, 470);
    float car_theta = 0.0f;

    while (!WindowShouldClose()) {
        update_camera(camera);

        if(IsMouseButtonPressed(MOUSE_BUTTON_RIGHT)) {
            Vector2 pos = GetScreenToWorld2D(GetMousePosition(), camera);
            points.emplace_back(pos.x, pos.y);
        }

        if(IsKeyPressed(KEY_D)) {
            points.clear();
        }

        auto edges = delaunay::triangulate(points);
        auto tris = delaunay::get_triangles(points);

        //auto middle_points = gen_middle_points(edges);
        //auto target_points = find_trajectory(car_position, middle_points);

        BeginDrawing();
            ClearBackground(RAYWHITE);
            BeginMode2D(camera);
                draw_grid();
                //rlLoadIdentity();
                draw_triangles(tris);

                draw_delaunay(edges);

                //draw_points(points, BLACK, 2);
                //draw_points(middle_points, BLUE, 1);
                draw_car(car_position, car_theta);
               // draw_points(target_points, ORANGE, 4);
            EndMode2D();
        EndDrawing();
    }

    CloseWindow();
    return 0;
}