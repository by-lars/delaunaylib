#ifndef AS_DATA_RECORDER_DELAUNAY_HPP
#define AS_DATA_RECORDER_DELAUNAY_HPP

#include "delaunay/quad_edge.hpp"
#include "delaunay/gsl.hpp"
#include "delaunay/point.hpp"

#include <cmath>
#include <vector>

namespace delaunay {
    /**
     * This class implements the divide and conquer delaunay triangulation algorithm as described in
     * "Primitives for the Manipulation of General Subdivisions and the Computation of Voronoi Diagrams"
     * by LEONIDA GUIBAS and JORGE STOLFI
     */
    class Delaunay {
      public:
        static auto triangulate(std::vector<point_t>& points) -> Delaunay;

        /**
         * Destructor, deletes all edges
         */
        ~Delaunay();

        /**
         * Get the generated primary edges
         * This is only filled after calling triangulate
         * @return vector of QuadEdge* (primary edges)
         */
        auto get_primary_edges() -> std::vector<QuadEdge *> const &;

        /**
         * Get the generated dual edges
         * This is only filled after calling triangulate
         * @return vector of QuadEdge* (dual edges)
         */
        auto get_dual_edges() -> std::vector<QuadEdge *> const &;

      private:
        /**
        * Constructor, runs algorithm
        */
        explicit Delaunay(std::vector<point_t>& points);

        /**
         * The recursive delaunay algorithm
         * @param points List of points to triangulate
         * @param start Offset in points list
         * @param length Length in poitns list
         * @return Left(=second) and Right(=first) most Edge
         */
        auto delaunay_divide_and_conquer(std::vector<point_t> const &points, std::size_t start, std::size_t length)
            -> std::pair<QuadEdge *, QuadEdge *>;

        /**
         * Build a triangle out of 3 points
         * @param start first point of the triangle (index into points array)
         * @param points array of points
         * @return Left(=second) and Right(=first) most Edge
         */
        auto build_triangle(std::size_t start, std::vector<point_t> const &points) -> std::pair<QuadEdge *, QuadEdge *>;

        /**
         * Merge the halves of the delaunay graph from divide and conquer
         * @param ldo left edge
         * @param ldi left edge
         * @param rdi right edge
         * @param rdo right edge
         * @return Left(=second) and Right(=first) most Edge
         */
        auto merge(QuadEdge *ldo, QuadEdge *ldi, QuadEdge *rdi, QuadEdge *rdo) -> std::pair<QuadEdge *, QuadEdge *>;

        /**
         * Computes the lowest common tangent of both halves
         * @param ldi left halve
         * @param rdi right halve
         * @return Left(=second) and Right(=first) most Edge
         */
        static auto compute_lowest_common_tangent(QuadEdge *ldi, QuadEdge *rdi) -> std::pair<QuadEdge *, QuadEdge *>;

        /**
         * Creates the dual edges of the graph (vornoi diagram)
         * starting the build at the right most edge
         * @param start The edge to start the build from  (should be the right most edge)
         */
        static void calculate_vornoi_graph(QuadEdge *start);

        /**
         * Create a new edge, and its QuadEdge entries
         * @param origin the start point of the edge
         * @param destination the end point of the edge
         * @return QuadEdge* to the new edge
         */
        auto make_edge(point_t const &origin, point_t const &destination) -> QuadEdge *;

        /**
         * Deletes an edge out a ring
         * This can cause the ring to fall
         * apart into two separate pieces
         * @param e Edge to delete
         * @return void
         */
        static void delete_edge(QuadEdge *e);

        /**
         * Splice is a fundamental operator on the QuadEdge data
         * structure used to alter the connections of the rings
         *
         * It performs the following operations depending on
         * 1. if the two rings are distinct, Splice will combine them into one
         * 2. if the two are exactly the same ring, Splice will break it in two separate pieces
         * 3. f the two are the same ring taken with opposite orientations, Splice will Flip (and reverse the order) of
         * a segment of that ring.
         * @param a first edge
         * @param b second edge
         * @return void
         */
        static void splice_edges(QuadEdge *a, QuadEdge *b);

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
        auto connect_edges(QuadEdge *a, QuadEdge *b) -> QuadEdge *;

        /**
         * Vector to hold all pointers to ensure memory safety (all primary edges)
         */
        std::vector<gsl::owner<QuadEdge *>> primary_edges;

        /**
         * Vector to hold all pointers to ensure memory safety (all sym primary edges)
         */
        std::vector<gsl::owner<QuadEdge *>> primary_sym_edges;

        /**
         * Vector to hold all pointers to ensure memory safety (all dual edges)
         */
        std::vector<gsl::owner<QuadEdge *>> dual_edges;

        /**
         * Vector to hold all pointers to ensure memory safety (all sym dual edges)
         */
        std::vector<gsl::owner<QuadEdge *>> dual_sym_edges;

      public:
        /**
         * The class should not be moved or copied
         * @param other other
         */
        Delaunay(Delaunay &&other) = delete;

        /**
         * The class should not be moved or copied
         * @param other other
         */
        Delaunay(Delaunay &other) = delete;

        /**
         * The class should not be moved or copied
         * @param other other
         */
        auto operator=(Delaunay &other) -> Delaunay & = delete;

        /**
         * The class should not be moved or copied
         * @param other other
         */
        auto operator=(Delaunay &&other) -> Delaunay & = delete;
    };

}// namespace analyser

#endif// AS_DATA_RECORDER_DELAUNAY_HPP
