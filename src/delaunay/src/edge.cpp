#include "edge.hpp"

namespace delaunay {
    Edge::Edge(delaunay::point_ref_t origin, delaunay::point_ref_t destination, int data) : origin(origin), destination(destination), data(data) {}
}