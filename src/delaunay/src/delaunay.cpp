#include "delaunay.hpp"
#include "triangulator.hpp"

#include <algorithm>

namespace delaunay {
    std::vector<Edge*> Edge::edges;

    Point::Point(float x, float y) : x(x), y(y) {}

    bool Point::operator==(const Point &other) const {
        return  this->x == other.x && this->y == other.y;
    }

    Edge::Edge(point_ref_t start, point_ref_t end) : start(start), end(end) {
        this->orbit_next = this;
        this->orbit_prev = this;
        this->sym_edge = nullptr;
        this->data = 0;
    }

    auto Edge::sym() -> Edge* {
        if(this->sym_edge == nullptr) {
            this->sym_edge = Edge::make(this->end, this->start);
        }

        return this->sym_edge;
    }

    void Edge::splice(Edge* a, Edge* b) {
        a->orbit_next->orbit_prev = b;
        b->orbit_next->orbit_prev = a;

        auto b_next = b->orbit_next;
        auto a_next = a->orbit_next;

        a->orbit_next = b_next;
        b->orbit_next = a_next;
    }

    bool Edge::is_point_on_right(point_ref_t point) const {
        return counter_clock_wise(point, this->end, this->start);
    }

    bool Edge::is_point_on_left(point_ref_t point) const {
        return counter_clock_wise(point, this->start, this->end);
    }

    Edge* Edge::connect(Edge* a, Edge* b) {
        Edge* new_edge = Edge::make(a->end, b->start);
        Edge::splice(new_edge, a->sym()->orbit_prev);
        Edge::splice(new_edge->sym(), b);
        return new_edge;
    }

    Edge *Edge::make(point_ref_t start, point_ref_t end) {
        Edge* e = new Edge(start, end);
        Edge::edges.push_back(e);
        return e;
    }

    void Edge::delete_edge(Edge* e) {
        Edge::splice(e, e->orbit_prev);
        Edge::splice(e->sym(), e->sym()->orbit_prev);
        e->data = -1;
    }

    std::vector<Edge*> triangulate(points_t points) {
        if(points.size() < 3) {
            return {};
        }

        Edge::edges.clear();


        std::stable_sort(points.begin(), points.end(), [](point_ref_t a, point_ref_t b) {
            return a.x > b.x;
        });

        Triangulator tri;
        tri.triangulate(points, 0, points.size());

        return Edge::edges;
    }

    bool counter_clock_wise(point_ref_t a, point_ref_t b, point_ref_t c) {
        //       | a.x a.y 1 |
        // |A| = | b.x b.y 1 | > 0
        //       | c.x c.y 1 |
        //
        // The points are in counterclockwise order
        // if the determinant of the matrix is grater than 0
        //
        // |A| = a.x * (b.y * 1 - 1 * c.y) - a.y * (b.x * 1 - 1 * c.x) + 1 * (b.x * c.y - b.y * c.x)

        float det = a.x * (b.y - c.y) - a.y * (b.x - c.x) + (b.x * c.y - b.y * c.x);
        return det > 0;
    }

    bool in_circle(point_ref_t a, point_ref_t b, point_ref_t c, point_ref_t d) {
        //       | a.x a.y a.x² + a.y² 1 |
        // |A| = | b.x b.y b.x² + b.y² 1 |
        //       | c.x c.y c.x² + c.y² 1 |
        //       | d.x d.y d.x² + d.y² 1 |
        //
        // |A| > 0 if d is in the circle defined by (a, b, c)

        float a1 = a.x-d.x;
        float a2 = a.y-d.y;

        float b1 = b.x-d.x;
        float b2 = b.y-d.y;

        float c1 = c.x-d.x;
        float c2 = c.y-d.y;

        float a3 = a1*a1 + a2*a2;;
        float b3 = b1*b1 + b2*b2;
        float c3 = c1*c1 + c2*c2;

        float det = a1*b2*c3 + a2*b3*c1 + a3*b1*c2 - (a3*b2*c1 + a1*b3*c2 + a2*b1*c3);

        return det < 0;
    }
}