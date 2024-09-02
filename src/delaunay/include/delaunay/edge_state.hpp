//
// Created by lars on 1/24/24.
//

#ifndef PATH_PLANNER_STANDALONE_EDGE_STATE_HPP
#define PATH_PLANNER_STANDALONE_EDGE_STATE_HPP

#include <cstdint>

namespace delaunay {
    /**
     * Describes how far in the creating of the graphs the edge is
     */
    enum class EdgeState : std::uint8_t {
        /**
         * Edge is created, but Origin and Destination is not set
         */
        DELETED = 0,

        /**
         * Origin and Destination is set
         */
        INITIALIZED = 1,

        /**
         * Edge was visited during Post Processing
         */
        PROCESSED = 2,
    };
}// namespace analyser

#endif// PATH_PLANNER_STANDALONE_EDGE_STATE_HPP
