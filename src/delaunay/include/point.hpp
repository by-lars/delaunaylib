#ifndef DELAUNAY_POINT_H
#define DELAUNAY_POINT_H

#include <vector>

namespace delaunay {
    struct Point {
        Point(float x, float y);

        bool operator==(const Point& other) const;

        float x;
        float y;

        [[nodiscard]] static bool counter_clock_wise(const Point& a, const Point& b, const Point& c);
        [[nodiscard]] static bool in_circle(const Point& a, const Point& b, const Point& c, const Point& d);
    };

    using point_t = Point;
    using point_ref_t = const Point&;
    using points_t = std::vector<point_t>;
    using points_ref_t = const points_t&;
}


#endif //DELAUNAY_POINT_H
