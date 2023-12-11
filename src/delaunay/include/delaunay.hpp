#ifndef DELAUNAY_DELAUNAY_HPP
#define DELAUNAY_DELAUNAY_HPP

#include <vector>
#include <array>

#include "edge.hpp"
#include "point.hpp"

namespace delaunay {
    std::vector<Edge> triangulate(points_t points);
}


#endif //DELAUNAY_DELAUNAY_HPP
