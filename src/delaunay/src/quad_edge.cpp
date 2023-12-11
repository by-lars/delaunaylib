#include "quad_edge.hpp"

namespace delaunay {
    std::vector<QuadEdge*> QuadEdge::s_edges;

    QuadEdge *delaunay::QuadEdge::make(point_ref_t origin, point_ref_t destination) {
        QuadEdge* q0 = new QuadEdge(origin);
        QuadEdge* q1 = new QuadEdge({0, 0});
        QuadEdge* q2 = new QuadEdge(destination);
        QuadEdge* q3 = new QuadEdge({0,0});

        q0->p_onext = q0; q2->p_onext = q2;
        q1->p_onext = q3; q3->p_onext = q1;

        q0->p_rot = q1; q1->p_rot = q2;
        q2->p_rot = q3; q3->p_rot = q0;

        s_edges.push_back(q0);
        s_edges.push_back(q1);
        s_edges.push_back(q2);
        s_edges.push_back(q3);

        return q0;
    }

    void QuadEdge::splice(QuadEdge *a, QuadEdge *b) {
        auto alpha = a->p_onext->p_rot;
        auto beta = b->p_onext->p_rot;

        auto a_onext = a->p_onext;
        auto b_onext = b->p_onext;
        auto alpha_onext = alpha->p_onext;
        auto beta_onext = beta->p_onext;

        a->p_onext = b_onext;
        b->p_onext = a_onext;
        alpha->p_onext = beta_onext;
        beta->p_onext = alpha_onext;
    }

    QuadEdge *QuadEdge::connect(QuadEdge *a, QuadEdge *b) {
        auto new_edge = QuadEdge::make(a->destination(), b->origin());
        QuadEdge::splice(new_edge, a->left_face_next());
        QuadEdge::splice(new_edge->sym(), b);
        return new_edge;
    }

    void QuadEdge::delete_edge(QuadEdge *e) {
        splice(e, e->orbit_prev());
        splice(e->sym(), e->sym()->orbit_prev());
        e->data = -1;
        e->sym()->data = -1;
    }

    QuadEdge::QuadEdge(point_ref_t origin) : m_origin(origin){
        p_onext = this;
        p_rot = this;
        data = 4;
    }

    point_t QuadEdge::origin() {
        return m_origin;
    }

    point_t QuadEdge::destination() {
        return this->sym()->origin();
    }

    QuadEdge *QuadEdge::sym() {
        //e Sym = e Rot²
        return this->p_rot->p_rot;
    }

    QuadEdge *QuadEdge::orbit_next() {
        //e Oprev = e Onext⁻¹ = e Rot Onext Rot
        return this->p_onext;
    }

    QuadEdge *QuadEdge::orbit_prev() {
        //e Oprev = e Onext⁻¹ = e Rot Onext Rot
        return this->p_rot->p_onext->p_rot;
    }

    QuadEdge *QuadEdge::left_face_next() {
        // e Lnext = e Rot⁻¹ Onext Rot,
        return this->p_rot->p_rot->p_rot->p_onext->p_rot;
    }

    QuadEdge *QuadEdge::left_face_prev() {
        // e Lprev = e Lnext⁻¹ = e Onext Sym
        return this->p_onext->sym();
    }

    QuadEdge *QuadEdge::right_face_next() {
        // e Rnext = e Rot Onext Rot⁻¹
        return this->p_rot->p_onext->p_rot->p_rot->p_rot;
    }

    QuadEdge *QuadEdge::right_face_prev() {
        // e Rprev = e Rnext⁻¹ = e Sym Onext
        return this->sym()->p_onext;
    }

    bool QuadEdge::is_point_on_right(point_ref_t point) {
        return Point::counter_clock_wise(point, this->destination(), this->origin());
    }

    bool QuadEdge::is_point_on_left(point_ref_t point) {
        return Point::counter_clock_wise(point, this->origin(), this->destination());
    }
}
