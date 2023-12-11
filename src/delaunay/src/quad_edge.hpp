#ifndef DELAUNAY_QUAD_EDGE_H
#define DELAUNAY_QUAD_EDGE_H

#include <vector>
#include "point.hpp"

namespace delaunay {
    class QuadEdge {
        QuadEdge* p_onext;
        QuadEdge* p_rot;
        point_t m_origin;
    public:

        static std::vector<QuadEdge*> s_edges;

        static QuadEdge* make(point_ref_t origin, point_ref_t destination);
        static void splice(QuadEdge* a, QuadEdge* b);
        static QuadEdge* connect(QuadEdge* a, QuadEdge* b);
        static void delete_edge(QuadEdge* e);

        int data;

    public:
        explicit QuadEdge(point_ref_t origin);

        [[nodiscard]] bool is_point_on_right(point_ref_t point);
        [[nodiscard]] bool is_point_on_left(point_ref_t point);

        point_t origin();
        point_t destination();
        QuadEdge* sym();
        QuadEdge* orbit_next();
        QuadEdge* orbit_prev();
        QuadEdge* left_face_next();
        QuadEdge* left_face_prev();
        QuadEdge* right_face_next();
        QuadEdge* right_face_prev();
    };
}



#endif //DELAUNAY_QUAD_EDGE_H
