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

        inline delaunay::point_t circumcenter() const {
            // https://en.wikipedia.org/wiki/Circumcircle
            // See Cartesian Coordiantes section

            point_t a = this->a->origin();
            point_t b = this->b->origin();
            point_t c = this->c->origin();

            float a_len_sqr = a.x * a.x + a.y * a.y;
            float b_len_sqr = b.x * b.x + b.y * b.y;
            float c_len_sqr = c.x * c.x + c.y * c.y;

            float d = 2.0f * (a.x * (b.y - c.y) + b.x * (c.y - a.y) + c.x * (a.y - b.y));
            float u_x = (a_len_sqr * (b.y - c.y) + b_len_sqr * (c.y - a.y) + c_len_sqr * (a.y - b.y)) / d;
            float u_y = (a_len_sqr * (c.x - b.x) + b_len_sqr * (a.x - c.x) + c_len_sqr * (b.x - a.x)) / d;

            return {u_x,u_y};
        }
    };

    HalfEdge* triangulate_points(points_t points);
    std::vector<Triangle> get_triangles(points_t points);
}


#endif //DELAUNAY_DELAUNAY_HPP
