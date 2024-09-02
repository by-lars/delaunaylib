#ifndef DELAUNAY_POINT_H
#define DELAUNAY_POINT_H

#include <vector>
#include "delaunay/types.hpp"

namespace delaunay {
    struct Point {
        Point(scalar_t x, scalar_t y);

        bool operator==(const Point& other) const;

        scalar_t x;
        scalar_t y;

        [[nodiscard]] static bool counter_clock_wise(const Point& a, const Point& b, const Point& c);
        [[nodiscard]] static bool in_circle(const Point& a, const Point& b, const Point& c, const Point& d);
        [[nodiscard]] static auto circumcenter(Point const &point_a, Point const &point_b, Point const &point_c) -> Point;
    };

    using point_t = Point;
}


#endif //DELAUNAY_POINT_H
