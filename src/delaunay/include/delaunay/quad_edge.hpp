#ifndef AS_DATA_RECORDER_QUAD_EDGE_HPP
#define AS_DATA_RECORDER_QUAD_EDGE_HPP

#include "delaunay/edge_state.hpp"
#include "delaunay/point.hpp"
#include "delaunay/types.hpp"

#include <vector>

namespace delaunay {
    class Delaunay;

    /**
     * This class implements the QuadEdge Data structure as described in
     * "Primitives for the Manipulation of General Subdivisions
     *  and the Computation of Voronoi Diagrams" by LEONIDA GUIBAS and JORGE STOLFI
     *
     * The QuadEdge data structure is basically a giant linked list, giving quick access
     * to the primal and dual. In this context the primal is the Delaunay Triangulation while the
     * dual is the Voronoi Diagram. This also makes navigating and modifying the mesh really easy.
     *
     * An Edge is not only represented by its simple origin and destination, but is rather split into
     * 4 half edges, representing the symmetric edge, the dual edge, and the symmetric dual edge, and
     * the edges sharing the same origin. Thus navigating around a vertex or a face is quite simple.
     *
     *
     * A movement in the next direction is always in the counter clockwise direction
     * A movement in the prev direction is always in the clockwise direction.
     *
     * Read more here: https://www.cs.cmu.edu/afs/andrew/scs/cs/15-463/2001/pub/src/a2/quadedge.html
     */
    struct QuadEdge {
        /**
         * Constructor setting up the default links
         * @param origin origin point of this half edge
         */
        explicit QuadEdge(const point_t& origin);

        /**
         * The origin or start point of this edge
         * @return origin point
         */
        auto origin() -> point_t const &;

        /**
         * The destination or end point of this edge
         * Equivalent to this->sym()->origin()
         * @return destination point
         */
        auto destination() -> point_t const &;

        /**
         * Gets the QuadEdge with the same orientation but opposite direction
         * @return The symmetrical QuadEdge to this one
         */
        auto sym() -> QuadEdge *;

        /**
         * Rotates around the Edges of the QuadEdge data structure, switching between
         * the dual and the primal edge. Rotating the Edge by 90° counterclockwise.
         *
         * The edge eRot is the dual of the edge e, direct from eRight to eLeft (Pointing to the left Face of e).
         * eRot = eFlipDual = eDualFlipSym
         * eSym = eRot²
         *
         * @return the next counter clockwise edge in the QuadEdge data structure (dual edge if applied to primal edge,
         * primal edge if applied to dual edge)
         */
        auto rot() -> QuadEdge *;

        /**
         * eRot⁻¹ = eRot³, same as rot but rotating clockwise.
         * The Edge eRot⁻¹ is the dual Edge of e, pointing to the right Face of e
         * @return the next clockwise edge in the QuadEdge data structure
         */
        auto inv_rot() -> QuadEdge *;

        /**
         * Counter clockwise rotation around the origin vertex of this edge
         * @return the next edge in counter clockwise direction of this edges origin vertex
         */
        auto orbit_next() -> QuadEdge *;

        /**
         * Clockwise rotation around the origin vertex of this edge
         * @return the next edge in clockwise direction of this edges origin vertex
         */
        auto orbit_prev() -> QuadEdge *;

        /**
         * The next (moving counter clockwise) edge in the face that is on the left to this one.
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eLeft in the counter clockwise direction.
         * @return next edge in the left face
         */
        auto left_face_next() -> QuadEdge *;

        /**
         * The prev (moving clockwise) edge in the face that is on the left to this one
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eLeft in the clockwise direction.
         * @return prev edge in the left face
         */
        auto left_face_prev() -> QuadEdge *;

        /**
         * The next (moving counter clockwise) edge in the face that is on the right to this one.
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eRight in the counter clockwise direction.
         * @return next edge in the right face
         */
        auto right_face_next() -> QuadEdge *;

        /**
         * The prev (moving clockwise) edge in the face that is on the right to this one
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eRight in the clockwise direction.
         * @return prev edge in the right face
         */
        auto right_face_prev() -> QuadEdge *;

        /**
         * Returns true if the specified point is to the right of this edge
         * @param point point to check
         * @return whether or not the point is to the right of this edge
         */
        auto is_point_on_right(point_t const &point) -> bool;

        /**
         * Returns true if the specified point is to the left of this edge
         * @param point point to check
         * @return whether or not the point is to the left of this edge
         */
        auto is_point_on_left(point_t const &point) -> bool;


        /**
         * If this edge is deleted / usable
         * @return true if this edge is usable (has origin and destination)
         */
        auto is_deleted() -> bool;

        /**
         * State of the Edge, e.g. in which stage of calculation (delaunay / vornoi) it is.
         */
        EdgeState state;

      private:
        /**
         * Origin point of this half edge
         */
        point_t m_origin;

        /**
         * Onext pointer
         * Points to the next edge in the edge ring / face this edge belongs to
         */
        QuadEdge *p_onext;

        /**
         * rot pointer
         * Points to the next edge in the QuadEdge data structure
         */
        QuadEdge *p_rot;

        friend class delaunay::Delaunay;
    };
}// namespace analyser
#endif// AS_DATA_RECORDER_QUAD_EDGE_HPP
