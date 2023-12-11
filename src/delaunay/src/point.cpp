#include "point.hpp"

namespace delaunay {
    Point::Point(float x, float y) : x(x), y(y) {}

    bool Point::operator==(const Point &other) const {
        return  this->x == other.x && this->y == other.y;
    }

    bool Point::counter_clock_wise(const Point &a, const Point &b, const Point &c) {
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

    bool Point::in_circle(const Point &a, const Point &b, const Point &c, const Point &d) {
        //       | a.x a.y a.x² + a.y² 1 |   | a.x - d.x  a.y-d.y  (a.x-d.x)² + (a.y-d.y)² |
        // |A| = | b.x b.y b.x² + b.y² 1 | = | b.x - d.x  b.y-d.y  (b.x-d.x)² + (b.y-d.y)² |
        //       | c.x c.y c.x² + c.y² 1 |   | c.x - d.x  c.y-d.y  (c.x-d.x)² + (c.y-d.y)² |
        //       | d.x d.y d.x² + d.y² 1 |
        //
        // |A| > 0 if d is in the circle defined by (a, b, c)
        // The expensive determinant of this 4x4 Matrix can be
        // simplified to a 3x3 determinant using using: https://www.cs.cmu.edu/~quake/robust.html

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

        return det > 0;
    }
}

