#include <gtest/gtest.h>

#include "delaunay.hpp"
#include "point.hpp"
#include "quad_edge.hpp"

/****************************
 * CCW / Counter Clock Wise *
 ****************************/
TEST(GeometricPrimitives, CounterClockwise) {
    delaunay::Point a(0, 0);
    delaunay::Point b(0, 1);
    delaunay::Point c(-1, 0);

    ASSERT_TRUE(delaunay::Point::counter_clock_wise(a,b,c));
}

TEST(GeometricPrimitives, NotCounterClockwise) {
    delaunay::Point a(0, 0);
    delaunay::Point b(0, 1);
    delaunay::Point c(1, 0);

    ASSERT_FALSE(delaunay::Point::counter_clock_wise(a,b,c));
}

TEST(GeometricPrimitives, CounterClockwiseBigNumbers) {
    delaunay::Point a(0, 0);
    delaunay::Point b(0, std::numeric_limits<float>::max());
    delaunay::Point c(std::numeric_limits<float>::lowest(), 0);

    ASSERT_TRUE(delaunay::Point::counter_clock_wise(a,b,c));
}

TEST(GeometricPrimitives, CounterClockwiseSmallNumbers) {
    delaunay::Point a(0, -0.000001f);
    delaunay::Point b(0, 0.000001f);
    delaunay::Point c(-0.00001f, 0);

    ASSERT_TRUE(delaunay::Point::counter_clock_wise(a,b,c));
}

TEST(GeometricPrimitives, ClockwiseSmallNumbers) {
    delaunay::Point a(0, -0.000001f);
    delaunay::Point b(0, 0.000001f);
    delaunay::Point c(0.00001f, 0);

    ASSERT_FALSE(delaunay::Point::counter_clock_wise(a,b,c));
}

/*******************
* LeftOf / RightOf *
********************/
TEST(GeometricPrimitives, LeftOf) {
    delaunay::Point a(0, 0);
    delaunay::Point b(0, 1);
    delaunay::Point c(-1, 0.5f);

    delaunay::HalfEdge* edge = delaunay::HalfEdge::make(a, b);
    ASSERT_TRUE(edge->is_point_on_left(c));
    ASSERT_FALSE(edge->is_point_on_right(c));
}