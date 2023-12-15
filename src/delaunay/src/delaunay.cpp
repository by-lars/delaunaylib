#include "delaunay.hpp"
#include "quad_edge.hpp"

#include <algorithm>
#include <iostream>
#include <stack>

namespace delaunay {

    static std::pair<HalfEdge*, HalfEdge*> delaunay_divide_and_conquer(points_ref_t points, std::size_t start, std::size_t length) {
        if(length==2) {
            auto a = HalfEdge::make(points[start], points[start + 1]);
            a->data = 2;
            a->sym()->data = 2;
            return {a, a->sym()};
        }

        // Gen a triangle, if 3 points are remaining
        if(length==3) {
            const auto& p1 = points[start];
            const auto& p2 = points[start+1];
            const auto& p3 = points[start+2];

            auto a = HalfEdge::make(p1, p2);
            HalfEdge* b = HalfEdge::make(p2, p3);
            HalfEdge::splice(a->sym(), b);

            a->data = 3;
            a->sym()->data = 3;
            b->data = 3;
            b->sym()->data = 3;

            if(Point::counter_clock_wise(p1, p2, p3)) {
                auto c = HalfEdge::connect(b, a);
                c->data = 3;
                c->sym()->data = 3;
                return {a, b->sym()};
            } else if(Point::counter_clock_wise(p1, p3, p2)) {
                auto c = HalfEdge::connect(b, a);
                c->data = 3;
                c->sym()->data = 3;
                return {c->sym(), c};
            } else { // The three points are collinear
                return {a,b->sym()};
            }
        }

        // Divide and CONQUER
        int off = length % 2 == 0 ? 0 : 1; // Adjust for uneven lengths of array
        auto left = delaunay_divide_and_conquer(points, start, length / 2);
        auto right = delaunay_divide_and_conquer(points, start + length / 2, length / 2 + off);


        HalfEdge* ldo = left.first;
        HalfEdge* ldi = left.second;
        HalfEdge* rdi = right.first;
        HalfEdge* rdo = right.second;

        // Compute the lower common tangent of left and right
        while (true) {
            if(ldi->is_point_on_left(rdi->origin())) {
                ldi = ldi->left_face_next();
            } else if (rdi->is_point_on_right(ldi->origin())) {
                rdi = rdi->right_face_prev(); // eRprev
            } else {
                break;
            }
        }

        // Create the first base QuadEdge, from rdi.start to ldi.start
        HalfEdge* base = HalfEdge::connect(rdi->sym(), ldi);
        base->data = 1;
        base->sym()->data = 1;

        if(ldi->origin() == ldo->origin()) { ldo = base->sym(); }
        if(rdi->origin() == rdo->origin()) { rdo = base; }

        while (true) {
            // Merge
            HalfEdge* lcand = base->sym()->orbit_next();
            bool lcand_valid = base->is_point_on_right(lcand->destination()); // valid(e) = RightOf(e.dest, basel)

            if(lcand_valid) {
                while(Point::in_circle(base->destination(), base->origin(), lcand->destination(), lcand->orbit_next()->destination())) {
                    HalfEdge* temp = lcand->orbit_next();
                    HalfEdge::delete_edge(lcand);
                    lcand = temp;
                }
            }

            HalfEdge* rcand = base->orbit_prev();
            bool rcand_valid = base->is_point_on_right(rcand->destination()); // valid(e) = RightOf(e.dest, basel)

            if(rcand_valid) {
                while (Point::in_circle(base->destination(), base->origin(), rcand->destination(), rcand->orbit_prev()->destination())) {
                    HalfEdge* tmp = rcand->orbit_prev();
                    HalfEdge::delete_edge(rcand);
                    rcand = tmp;
                }
            }

            // Base must be the upper common tangent
            if(!lcand_valid && !rcand_valid) {
                break;
            }

            if(!lcand_valid || (rcand_valid && Point::in_circle(lcand->destination(), lcand->origin(), rcand->origin(), rcand->destination()))) {
                base = HalfEdge::connect(rcand, base->sym());
            } else {
                base = HalfEdge::connect(base->sym(), lcand->sym());
            }

        }

        return {ldo, rdo};
    }

    std::vector<Edge> triangulate(points_t points) {
        triangulate_points(points);

        std::vector<Edge> edges;

        for(const auto& edge : HalfEdge::s_edges) {
            if(edge->data == 1 || edge->data == 2 || edge->data == 3 || edge->data == 4) {
                edges.emplace_back(edge->origin(), edge->destination(), edge->data);
                edge->sym()->data = 0;
            }
        }

        return edges;
    }

    HalfEdge* triangulate_points(points_t points) {
        for(HalfEdge* e : HalfEdge::s_edges)  {
            delete e;
        }

        HalfEdge::s_edges.clear();

        if(points.size() < 3) {
            return {};
        }

        std::stable_sort(points.begin(), points.end(), [](point_ref_t a, point_ref_t b) {
            return a == b ? a.y > b.y : a.x > b.x;
        });

        auto delaunay = delaunay_divide_and_conquer(points, 0, points.size());
        return delaunay.first;
    }

    std::vector<Triangle> get_triangles(points_t points) {
        HalfEdge* starting_edge = triangulate_points(points);

        //visited = set()
        //stack = [startEdge]
        //
        //while stack is not empty:
        //  edge = stack.pop()
        //  triangle = [edge, edge.Lnext(), edge.Lnext().Lnext()]
        //
        //  if triangle not in visited:
        //      visited.add(triangle)
        //      processTriangle(triangle)
        //
        //  for e in triangle:
        //      oppositeEdge = e.Sym()
        //      if oppositeEdge not in stack:
        //          stack.push(oppositeEdge)

        std::vector<Triangle> triangles;

        while(starting_edge->is_point_on_left(starting_edge->orbit_next()->destination())) {
            starting_edge = starting_edge->orbit_next();
        }

        std::stack<HalfEdge*> to_visit;
        to_visit.emplace(starting_edge);



        while(!to_visit.empty()) {
            HalfEdge* e = to_visit.top();
            to_visit.pop();

            if(e->data == 0 || e->data == -1) {
                continue;
            }

            e->data = 0;

            if(e->is_point_on_left(e->left_face_next()->destination())) {
                Triangle t {e, e->left_face_next()->sym(), e->sym()};
                triangles.push_back(t);
            } else {
                Triangle t {e, e->sym(), e->left_face_next()->sym()};
                triangles.push_back(t);
            }


            to_visit.push(e->sym());
            to_visit.push(e->left_face_next());

        }

        return triangles;
    }
}