#ifndef DELAUNAY_TRIANGULATOR_HPP
#define DELAUNAY_TRIANGULATOR_HPP

#include "delaunay.hpp"

namespace delaunay {


    class Triangulator {
    public:
        std::pair<Edge*, Edge*> triangulate(points_ref_t points, std::size_t start, std::size_t length);


    private:
    };
}

#endif //DELAUNAY_TRIANGULATOR_HPP
