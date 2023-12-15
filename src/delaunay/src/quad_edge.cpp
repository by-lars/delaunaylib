#include "quad_edge.hpp"

namespace delaunay {
    std::vector<HalfEdge*> HalfEdge::s_edges;

    HalfEdge *delaunay::HalfEdge::make(point_ref_t origin, point_ref_t destination) {
        auto* q0 = new HalfEdge(origin);
        auto* q1 = new HalfEdge({0, 0});
        auto* q2 = new HalfEdge(destination);
        auto* q3 = new HalfEdge({0, 0});

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

    void HalfEdge::splice(HalfEdge *a, HalfEdge *b) {
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

    HalfEdge *HalfEdge::connect(HalfEdge *a, HalfEdge *b) {
        auto new_edge = HalfEdge::make(a->destination(), b->origin());
        HalfEdge::splice(new_edge, a->left_face_next());
        HalfEdge::splice(new_edge->sym(), b);
        return new_edge;
    }

    void HalfEdge::delete_edge(HalfEdge *e) {
        splice(e, e->orbit_prev());
        splice(e->sym(), e->sym()->orbit_prev());
        e->data = -1;
        e->sym()->data = -1;
    }

    HalfEdge::HalfEdge(point_ref_t origin) : m_origin(origin){
        p_onext = this;
        p_rot = this;
        data = 4;
    }

    point_t HalfEdge::origin() {
        return m_origin;
    }

    point_t HalfEdge::destination() {
        return this->sym()->origin();
    }

    HalfEdge *HalfEdge::sym() {
        //e Sym = e Rot²
        return this->p_rot->p_rot;
    }

    HalfEdge *HalfEdge::orbit_next() {
        //e Oprev = e Onext⁻¹ = e Rot Onext Rot
        return this->p_onext;
    }

    HalfEdge *HalfEdge::orbit_prev() {
        //e Oprev = e Onext⁻¹ = e Rot Onext Rot
        return this->p_rot->p_onext->p_rot;
    }

    HalfEdge *HalfEdge::left_face_next() {
        // e Lnext = e Rot⁻¹ Onext Rot,
        return this->p_rot->p_rot->p_rot->p_onext->p_rot;
    }

    HalfEdge *HalfEdge::left_face_prev() {
        // e Lprev = e Lnext⁻¹ = e Onext Sym
        return this->p_onext->sym();
    }

    HalfEdge *HalfEdge::right_face_next() {
        // e Rnext = e Rot Onext Rot⁻¹
        return this->p_rot->p_onext->p_rot->p_rot->p_rot;
    }

    HalfEdge *HalfEdge::rot() {
        // eRot
        return this->p_rot;
    }

    HalfEdge *HalfEdge::right_face_prev() {
        // e Rprev = e Rnext⁻¹ = e Sym Onext
        return this->sym()->p_onext;
    }

    bool HalfEdge::is_point_on_right(point_ref_t point) {
        return Point::counter_clock_wise(point, this->destination(), this->origin());
    }

    bool HalfEdge::is_point_on_left(point_ref_t point) {
        return Point::counter_clock_wise(point, this->origin(), this->destination());
    }

}
