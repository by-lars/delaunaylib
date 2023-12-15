#ifndef DELAUNAY_DELAUNAY_HPP
#define DELAUNAY_DELAUNAY_HPP

#include <vector>
#include <array>

#include "edge.hpp"
#include "point.hpp"
#include "quad_edge.hpp"

namespace delaunay {
    std::vector<Edge> triangulate(points_t points);

    struct Triangle {
        HalfEdge* a;
        HalfEdge* b;
        HalfEdge* c;
    };

    HalfEdge* triangulate_points(points_t points);
    std::vector<Triangle> get_triangles(points_t points);
}


#endif //DELAUNAY_DELAUNAY_HPP
