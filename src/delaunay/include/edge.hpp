#ifndef DELAUNAY_EDGE_HPP
#define DELAUNAY_EDGE_HPP

#include "point.hpp"

namespace delaunay {
    struct Edge {
        Edge(point_ref_t origin, point_ref_t destination, int data);

        point_t origin;
        point_t destination;
        int data;
    };
}

#endif //DELAUNAY_EDGE_HPP
