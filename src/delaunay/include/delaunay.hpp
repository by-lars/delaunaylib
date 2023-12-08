#ifndef DELAUNAY_DELAUNAY_HPP
#define DELAUNAY_DELAUNAY_HPP

#include <vector>

namespace delaunay {
    struct Point {
        Point(float x, float y);

        bool operator==(const Point& other) const;

        float x;
        float y;
    };

    using point_t = Point;
    using point_ref_t = const Point&;
    using points_t = std::vector<point_t>;
    using points_ref_t = const points_t&;

    bool counter_clock_wise(point_ref_t a, point_ref_t b, point_ref_t c);
    bool in_circle(point_ref_t a, point_ref_t b, point_ref_t c, point_ref_t d);

    class Edge {
    public:
        Edge(point_ref_t start, point_ref_t end);

        static Edge* make(point_ref_t start, point_ref_t end);
        static void splice(Edge* a, Edge* b);
        static Edge* connect(Edge* a, Edge* b);
        static void delete_edge(Edge* e);

        [[nodiscard]] auto sym() -> Edge*;
        bool is_point_on_right(point_ref_t point) const;

        bool is_point_on_left(point_ref_t point) const;

        /**
         * Next Edge in orbit - Counter Clock Wise Direction
         */
        Edge* orbit_next;

        /**
         * Previous Edge in orbit - Clock Wise Direction
         */
        Edge* orbit_prev;

        point_t start;

        point_t end;

        int data;

        static std::vector<Edge*> edges;
    private:
        Edge* sym_edge;
    };

    using edge_t = Edge;
    using edge_ref_t = const Edge&;
    using edges_t = std::vector<edge_t>;
    using edges_ref_r = const edge_t&;

    std::vector<Edge*> triangulate(points_t points);
}


#endif //DELAUNAY_DELAUNAY_HPP
