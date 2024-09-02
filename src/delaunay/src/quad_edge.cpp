#include "delaunay/quad_edge.hpp"

#include <limits>
#include <utility>

namespace delaunay {
    QuadEdge::QuadEdge(const point_t& origin) :
        state(EdgeState::DELETED),
        m_origin(std::move(origin)),
        p_onext(this),
        p_rot(this) {}

    auto QuadEdge::origin() -> point_t const & {
        return m_origin;
    }

    auto QuadEdge::destination() -> point_t const & {
        return this->sym()->origin();
    }

    auto QuadEdge::sym() -> QuadEdge * {
        // e Sym = e Rot²
        return this->p_rot->p_rot;
    }

    auto QuadEdge::orbit_next() -> QuadEdge * {
        // e Oprev = e Onext⁻¹ = e Rot Onext Rot
        return this->p_onext;
    }

    auto QuadEdge::orbit_prev() -> QuadEdge * {
        // e Oprev = e Onext⁻¹ = e Rot Onext Rot
        return this->p_rot->p_onext->p_rot;
    }

    auto QuadEdge::left_face_next() -> QuadEdge * {
        // e Lnext = e Rot⁻¹ Onext Rot,
        return this->inv_rot()->p_onext->p_rot;
    }

    auto QuadEdge::left_face_prev() -> QuadEdge * {
        // e Lprev = e Lnext⁻¹ = e Onext Sym
        return this->p_onext->sym();
    }

    auto QuadEdge::right_face_next() -> QuadEdge * {
        // e Rnext = e Rot Onext Rot⁻¹
        return this->p_rot->p_onext->inv_rot();
    }

    auto QuadEdge::rot() -> QuadEdge * {
        // eRot
        return this->p_rot;
    }

    auto QuadEdge::right_face_prev() -> QuadEdge * {
        // e Rprev = e Rnext⁻¹ = e Sym Onext
        return this->sym()->p_onext;
    }

    auto QuadEdge::inv_rot() -> QuadEdge * {
        // eRot⁻¹ = eRot³ = eRotRotRot
        return rot()->rot()->rot();
    }

    auto QuadEdge::is_point_on_right(point_t const &point) -> bool {
        return point_t::counter_clock_wise(point, this->destination(), this->origin());
    }

    auto QuadEdge::is_point_on_left(point_t const &point) -> bool {
        return point_t::counter_clock_wise(point, this->origin(), this->destination());
    }

    auto QuadEdge::is_deleted() -> bool {
        return this->state == EdgeState::DELETED || this->origin().x == std::numeric_limits<scalar_t>::infinity() ||
               this->destination().x == std::numeric_limits<scalar_t>::infinity() || this->sym() == nullptr;
    }

}// namespace analyser
