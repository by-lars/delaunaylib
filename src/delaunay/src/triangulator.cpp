#include "triangulator.hpp"
#include <iostream>

namespace delaunay {
    int Triangulator::counter = 1;

    std::pair<Edge*, Edge*> Triangulator::triangulate(delaunay::points_ref_t points, std::size_t start, std::size_t length) {


        // Gen a new edge, if only two points are remaning
        if(length==2) {
            Edge* a = Edge::make(points[start], points[start+1]);
            counter++;
            a->data = counter;
            a->sym()->data = counter;
            return {a, a->sym()};
        }

        // Gen a triangle, if 3 points are remaning
        if(length==3) {
            const auto& p1 = points[start];
            const auto& p2 = points[start+1];
            const auto& p3 = points[start+2];

            Edge* a = Edge::make(p1, p2);
            Edge* b = Edge::make(p2, p3);
            Edge::splice(a->sym(), b);

            counter++;
            a->data = counter;
            a->sym()->data = counter;
            b->data = counter;
            b->sym()->data = counter;

            if(counter_clock_wise(p1, p2, p3)) {
                auto c = Edge::connect(b, a);
                c->data = counter;
                c->sym()->data = counter;
                return {a, b->sym()};
            } else if(counter_clock_wise(p1, p3, p2)) {
                auto c = Edge::connect(b, a);
                c->data = counter;
                c->sym()->data = counter;
                return {c->sym(), c};
            } else { // The three points are collinear
                return {a,b->sym()};
            }
        }

        // Divide and CONQUER
        int off = length % 2 == 0 ? 0 : 1; // Adjust for uneven lengths of array
        auto left = triangulate(points, start, length / 2);
        auto right = triangulate(points, start + length / 2, length / 2 + off);

        Edge* ldo = left.first;
        Edge* ldi = left.second;
        Edge* rdi = right.first;
        Edge* rdo = right.second;

        // Compute the lower common tangent of left and right
        while (true) {
            if(ldi->is_point_on_left(rdi->start)) {
                ldi = ldi->sym()->orbit_next; // CHECK THIS
            } else if (rdi->is_point_on_right(ldi->start)) {
                rdi = rdi->sym()->orbit_prev; // CHECK THIS
            } else {
                break;
            }
        }

        // Create the first base edge, from rdi.start to ldi.start
        Edge* base = Edge::connect(rdi->sym(), ldi);
        base->data = 1;

        if(ldi->start == ldo->start) { ldo = base->sym(); }
        if(rdi->start == rdo->start) { rdo = base; }

        while (false) {
            // Merge
            Edge* lcand = base->sym()->orbit_next;
            bool lcand_valid = base->is_point_on_right(lcand->end); // valid(e) = RightOf(e.dest, basel)

            if(lcand_valid) {
                while(in_circle(base->end, base->start, lcand->end, lcand->orbit_next->end)) {
                    Edge* temp = lcand->orbit_next;
                    Edge::delete_edge(lcand);
                    lcand = temp;
                }
            }

            Edge* rcand = base->orbit_prev;
            bool rcand_valid = base->is_point_on_right(rcand->end); // valid(e) = RightOf(e.dest, basel)

            if(rcand_valid) {
                while (in_circle(base->end, base->start, rcand->end, rcand->orbit_prev->end)) {
                    Edge* tmp = rcand->orbit_prev;
                    Edge::delete_edge(rcand);
                    rcand = tmp;
                }
            }

            // Base must be the upper common tangent
            if(!lcand_valid && !rcand_valid) {
                break;
            }

            if(!lcand_valid || (rcand_valid && in_circle(lcand->end, lcand->start, rcand->start, rcand->end))) {
                base = Edge::connect(rcand, base->sym());
            } else {
                base = Edge::connect(base->sym(), lcand->sym());
            }

        }

        return {ldo, rdo};
    }
}