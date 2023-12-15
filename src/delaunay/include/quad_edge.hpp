#ifndef DELAUNAY_QUAD_EDGE_H
#define DELAUNAY_QUAD_EDGE_H

#include <vector>
#include "point.hpp"

namespace delaunay {
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
     */
    class HalfEdge {
    public:
        /**
         * Constructor setting up the default links
         * @param origin origin point of this half edge
         */
        explicit HalfEdge(point_ref_t origin);

        /**
         * The origin or start point of this edge
         * @return origin point
         */
        point_t origin();

        /**
         * The destination or end point of this edge
         * Equivalent to this->sym()->origin()
         * @return destination point
         */
        point_t destination();

        /**
         * Gets the HalfEdge with the same orientation but opposite direction
         * @return The symmetrical halfedge to this one
         */
        HalfEdge* sym();

        /**
         * Rotates around the Edges of the QuadEdge data structure, switching between
         * the dual and the primal edge
         *
         * The edge eRot is the dual of the edge e, direct from eRight to eLeft.
         * eRot = eFlipDual = eDualFlipSym
         * eSym = eRot²
         *
         * @return the next counter clockwise edge in the QuadEdge data structure
         */
        HalfEdge* rot();

        /**
         * Counter clockwise rotation around the origin vertex of this edge
         * @return the next edge in counter clockwise direction of this edges origin vertex
         */
        HalfEdge* orbit_next();

        /**
         * Clockwise rotation around the origin vertex of this edge
         * @return the next edge in clockwise direction of this edges origin vertex
         */
        HalfEdge* orbit_prev();

        /**
         * The next (moving counter clockwise) edge in the face that is on the left to this one.
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eLeft in the counter clockwise direction.
         * @return next edge in the left face
         */
        HalfEdge* left_face_next();

        /**
         * The prev (moving clockwise) edge in the face that is on the left to this one
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eLeft in the clockwise direction.
         * @return prev edge in the left face
         */
        HalfEdge* left_face_prev();

        /**
         * The next (moving counter clockwise) edge in the face that is on the right to this one.
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eRight in the counter clockwise direction.
         * @return next edge in the right face
         */
        HalfEdge* right_face_next();

        /**
         * The prev (moving clockwise) edge in the face that is on the right to this one
         * Meaning the returned edge is the first edge we encounter after this edge,
         * when moving along the boundary of the face F = eRight in the clockwise direction.
         * @return prev edge in the right face
         */
        HalfEdge* right_face_prev();

    public:
        /**
         * Returns true if the specified point is to the right of this edge
         * @param point point to check
         * @return whether or not the point is to the right of this edge
         */
        bool is_point_on_right(point_ref_t point);

        /**
         * Returns true if the specified point is to the left of this edge
         * @param point point to check
         * @return whether or not the point is to the left of this edge
         */
        bool is_point_on_left(point_ref_t point);

        /**
         * Create a new edge, and its QuadEdge entries
         * @param origin the start point of the edge
         * @param destination the end point of the edge
         * @return HalfEdge* to the new edge
         */
        static HalfEdge* make(point_ref_t origin, point_ref_t destination);

        /**
         * Splice is a fundamental operator on the QuadEdge data
         * structure used to alter the connections of the rings
         *
         * It performs the following operations depending on
         * 1. if the two rings are distinct, Splice will combine them into one
         * 2. if the two are exactly the same ring, Splice will break it in two separate pieces
         * 3. f the two are the same ring taken with opposite orientations, Splice will Flip (and reverse the order) of a segment of that ring.
         * @param a first edge
         * @param b second edge
         */
        static void splice(HalfEdge* a, HalfEdge* b);

        /**
         * Creates a new edge r, connecting the destination
         * of a to the origin of b, in such a way that aLeft = eLeft = bLeft
         *
         * eLeft being the Left face of an Edge e
         *
         * @param a first edge
         * @param b second edge
         * @return New edge e
         */
        static HalfEdge* connect(HalfEdge* a, HalfEdge* b);

        /**
         * Deletes an edge out a ring
         * This can cause the ring to fall
         * apart into two separate pieces
         * @param e Edge to delete
         */
        static void delete_edge(HalfEdge* e);

    public:
        static std::vector<HalfEdge*> s_edges;


        int data;

    private:
        /**
         * Onext pointer
         * Points to the next edge in the edge ring / face this edge belongs to
         */
        HalfEdge* p_onext;

        /**
         * rot pointer
         * Points to the next edge with the same origin as this one
         */
        HalfEdge* p_rot;

        /**
         * Origin point of this half edge
         */
        point_t m_origin;
    };

    using edge_t = HalfEdge;
    using edge_ref_r = const edge_t&;
}



#endif //DELAUNAY_QUAD_EDGE_H
