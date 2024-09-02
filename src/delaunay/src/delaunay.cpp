#include "delaunay/delaunay.hpp"
#include "delaunay/quad_edge.hpp"

#include <algorithm>
#include <cmath>
#include <stack>

namespace delaunay {
    auto Delaunay::triangulate(std::vector<point_t> &points) -> delaunay::Delaunay {
        return Delaunay(points);
    }

    Delaunay::Delaunay(std::vector<point_t> &points) {
        // Triangulation requires at least 3 Points
        if (points.size() < 3) {
            return;
        }

        // Sort points, as this is important for the divide and concquer algorithm to work
        std::stable_sort(points.begin(), points.end(), [](point_t const &a, point_t const &b) {
            return a.y > b.y;
        });

        std::stable_sort(points.begin(), points.end(), [](point_t const &a, point_t const &b) {
            return a.x > b.x;
        });

        // Remove duplicates, as they destroy the triangulation
        auto last = std::unique(points.begin(), points.end());
        points.erase(last, points.end());

        // Call recursive triangulation routine
        auto result = delaunay_divide_and_conquer(points, 0, points.size());

        // Create Vornoi Graph (starting at the right most edge
        calculate_vornoi_graph(result.first);
    }


    auto Delaunay::build_triangle(std::size_t start, std::vector<point_t> const &points)
        -> std::pair<QuadEdge *, QuadEdge *> {
        auto const &p1 = points[start];
        auto const &p2 = points[start + 1];
        auto const &p3 = points[start + 2];

        auto *a = make_edge(p1, p2);
        auto *b = make_edge(p2, p3);
        splice_edges(a->sym(), b);

        if (point_t::counter_clock_wise(p1, p2, p3)) {
            std::ignore = connect_edges(b, a);
            return {a, b->sym()};
        }

        if (point_t::counter_clock_wise(p1, p3, p2)) {
            auto *c = connect_edges(b, a);
            return {c->sym(), c};
        }

        // The three points are collinear
        return {a, b->sym()};
    }

    auto
    Delaunay::merge(QuadEdge *ldo, QuadEdge *ldi, QuadEdge *rdi, QuadEdge *rdo) -> std::pair<QuadEdge *, QuadEdge *> {
        // Create the first base QuadEdge, from rdi.start to ldi.start
        QuadEdge *base = connect_edges(rdi->sym(), ldi);

        if (ldi->origin() == ldo->origin()) {
            ldo = base->sym();
        }
        if (rdi->origin() == rdo->origin()) {
            rdo = base;
        }

        while (true) {
            // Merge
            QuadEdge *lcand = base->sym()->orbit_next();
            bool const lcand_valid = base->is_point_on_right(lcand->destination());// valid(e) = RightOf(e.dest, basel)

            if (lcand_valid) {
                while (point_t::in_circle(
                    base->destination(),
                    base->origin(),
                    lcand->destination(),
                    lcand->orbit_next()->destination()
                )) {
                    QuadEdge *temp = lcand->orbit_next();
                    delete_edge(lcand);
                    lcand = temp;
                }
            }

            QuadEdge *rcand = base->orbit_prev();
            bool const rcand_valid = base->is_point_on_right(rcand->destination());// valid(e) = RightOf(e.dest, basel)

            if (rcand_valid) {
                while (point_t::in_circle(
                    base->destination(),
                    base->origin(),
                    rcand->destination(),
                    rcand->orbit_prev()->destination()
                )) {
                    QuadEdge *tmp = rcand->orbit_prev();
                    delete_edge(rcand);
                    rcand = tmp;
                }
            }

            // Base must be the upper common tangent
            if (!lcand_valid && !rcand_valid) {
                break;
            }

            if (!lcand_valid ||
                (rcand_valid &&
                 point_t::in_circle(lcand->destination(), lcand->origin(), rcand->origin(), rcand->destination()))) {
                base = connect_edges(rcand, base->sym());
            } else {
                base = connect_edges(base->sym(), lcand->sym());
            }
        }

        return {ldo, rdo};
    }

    auto Delaunay::compute_lowest_common_tangent(QuadEdge *ldi, QuadEdge *rdi) -> std::pair<QuadEdge *, QuadEdge *> {
        // Compute the lower common tangent of left and right
        while (true) {
            if (ldi->is_point_on_left(rdi->origin())) {
                ldi = ldi->left_face_next();
            } else if (rdi->is_point_on_right(ldi->origin())) {
                rdi = rdi->right_face_prev();
            } else {
                break;
            }
        }

        return {ldi, rdi};
    }

    auto
    Delaunay::delaunay_divide_and_conquer(std::vector<point_t> const &points, std::size_t start, std::size_t length)
        -> std::pair<QuadEdge *, QuadEdge *> {
        // Build a single edge out of 2 points
        if (length == 2) {
            auto *a = make_edge(points[start], points[start + 1]);
            return {a, a->sym()};
        }

        // Build a triangle out of the three given points
        if (length == 3) {
            return build_triangle(start, points);
        }

        // Divide and CONQUER
        std::uint8_t const off = length % 2 == 0 ? 0 : 1;// Adjust for uneven lengths of array
        auto left = delaunay_divide_and_conquer(points, start, length / 2);
        auto right = delaunay_divide_and_conquer(points, start + length / 2, length / 2 + off);

        // Find lowest common tangent (lowest point) of both halves
        auto lowest = compute_lowest_common_tangent(left.second, right.first);

        // Merge both halves
        auto merge_result = merge(left.first, lowest.first, lowest.second, right.second);

        // Return result
        return merge_result;
    }

    auto Delaunay::get_primary_edges() -> std::vector<QuadEdge *> const & {
        return this->primary_edges;
    }

    auto Delaunay::get_dual_edges() -> std::vector<QuadEdge *> const & {
        return this->dual_edges;
    }

    void Delaunay::calculate_vornoi_graph(QuadEdge *start) {
        if (start == nullptr) {
            return;
        }

        std::stack<QuadEdge *> edges_to_visit;
        edges_to_visit.push(start);

        while (!edges_to_visit.empty()) {
            QuadEdge *current = edges_to_visit.top();
            edges_to_visit.pop();

            QuadEdge *a = current;
            QuadEdge *b = current->left_face_next();
            QuadEdge *c = current->left_face_next()->left_face_next();

            // If one of the edges was already visited, we don't want to create a triangle here as there already is one
            if (a->state == EdgeState::INITIALIZED && b->state == EdgeState::INITIALIZED &&
                c->state == EdgeState::INITIALIZED) {
                if (point_t::counter_clock_wise(a->origin(), b->origin(), c->origin())) {
                    point_t const circumcenter = point_t::circumcenter(a->origin(), b->origin(), c->origin());

                    a->inv_rot()->m_origin = circumcenter;
                    b->inv_rot()->m_origin = circumcenter;
                    c->inv_rot()->m_origin = circumcenter;

                    a->inv_rot()->state = EdgeState::INITIALIZED;
                    b->inv_rot()->state = EdgeState::INITIALIZED;
                    c->inv_rot()->state = EdgeState::INITIALIZED;
                }
            }

            // Mark current Edge as already visited
            current->state = EdgeState::PROCESSED;
            current->sym()->state = EdgeState::PROCESSED;

            // The edges a and b are chosen is such a way, that their left face is
            // not the same face as the triangle we just generated.

            // Edge of upper adjacent triangle pointing in counterclockwise direction of the next triangle
            QuadEdge *next_a = current->left_face_next()->sym();
            if (next_a->state == EdgeState::INITIALIZED) {
                edges_to_visit.push(next_a);
            }

            // Edge of lower adjacent triangle pointing in counterclockwise direction of the next triangle
            QuadEdge *next_b = current->orbit_next();
            if (next_b->state == EdgeState::INITIALIZED) {
                edges_to_visit.push(next_b);
            }
        }
    }


    auto Delaunay::make_edge(point_t const &origin, point_t const &destination) -> QuadEdge * {
        auto *primary = new QuadEdge(origin);
        auto *dual =
            new QuadEdge(point_t(std::numeric_limits<scalar_t>::infinity(), std::numeric_limits<scalar_t>::infinity()));
        auto *primary_sym = new QuadEdge(destination);
        auto *dual_sym =
            new QuadEdge(point_t(std::numeric_limits<scalar_t>::infinity(), std::numeric_limits<scalar_t>::infinity()));

        // The primary edges have a set origin and destination
        primary->state = EdgeState::INITIALIZED;
        primary_sym->state = EdgeState::INITIALIZED;

        primary->p_onext = primary;
        primary_sym->p_onext = primary_sym;
        dual->p_onext = dual_sym;
        dual_sym->p_onext = dual;

        primary->p_rot = dual;
        dual->p_rot = primary_sym;
        primary_sym->p_rot = dual_sym;
        dual_sym->p_rot = primary;

        primary_edges.emplace_back(primary);
        dual_edges.emplace_back(dual);
        primary_sym_edges.emplace_back(primary_sym);
        dual_sym_edges.emplace_back(dual_sym);

        return primary;
    }

    void Delaunay::delete_edge(QuadEdge *e) {
        splice_edges(e, e->orbit_prev());
        splice_edges(e->sym(), e->sym()->orbit_prev());
        e->state = EdgeState::DELETED;
        e->sym()->state = EdgeState::DELETED;
    }

    void Delaunay::splice_edges(QuadEdge *a, QuadEdge *b) {
        auto *alpha = a->p_onext->p_rot;
        auto *beta = b->p_onext->p_rot;

        auto *a_onext = a->p_onext;
        auto *b_onext = b->p_onext;
        auto *alpha_onext = alpha->p_onext;
        auto *beta_onext = beta->p_onext;

        a->p_onext = b_onext;
        b->p_onext = a_onext;
        alpha->p_onext = beta_onext;
        beta->p_onext = alpha_onext;
    }

    auto Delaunay::connect_edges(QuadEdge *a, QuadEdge *b) -> QuadEdge * {
        auto *new_edge = make_edge(a->destination(), b->origin());
        splice_edges(new_edge, a->left_face_next());
        splice_edges(new_edge->sym(), b);
        return new_edge;
    }

    Delaunay::~Delaunay() {
        for (gsl::owner<QuadEdge *> edge : primary_edges) {
            delete edge;
        }
        for (gsl::owner<QuadEdge *> edge : dual_edges) {
            delete edge;
        }
        for (gsl::owner<QuadEdge *> edge : primary_sym_edges) {
            delete edge;
        }
        for (gsl::owner<QuadEdge *> edge : dual_sym_edges) {
            delete edge;
        }

        primary_edges.clear();
        dual_edges.clear();
        primary_sym_edges.clear();
        dual_sym_edges.clear();
    }
}// namespace analyser
