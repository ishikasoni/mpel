/**
 * \file builtins.hpp
 * \brief This file contains the declarations of all built in components
 */
#ifndef MPEL_BUILTINS_H
#define MPEL_BUILTINS_H

#include "planner.hpp"
#include "types.hpp"

namespace mpel {
/// All the builtin components are defined within this namespace
namespace builtin {

    /// metric contains functors for calculating various kinds of distances
    namespace metric {
        /// \f$distance((x_1,y_1),(x_2,y_2)) = \sqrt{(x_2-x_1)^2+(y_2-y_1)^2}\f$
        struct euclidean {
            double operator()(PointRef a, PointRef b);
        };

        /// \f$distance((x_1,y_1),(x_2,y_2)) = |x_2-x_1|+|y_2-y_1|\f$
        struct manhattan {
            double operator()(PointRef a, PointRef b);
        };

        /// \f$distance((x_1,y_1),(x_2,y_2)) = \max(|x_2-x_1|,|y_2-y_1|)\f$
        struct chebychev {
            double operator()(PointRef a, PointRef b);
        };
    }

    /// This namespace contains definitions of builtin graph builders
    namespace graph_builder {
        /**
         * \brief The empty graph builder.
         *
         * This is used in cases when a planner expects a graph_builder but
         * we don't actually want to construct a graph. Consider the case of
         * motion planning using only artificial potential fields as an example.
         *
         * \snippet pot_planner.cpp Planner configuration
         *
         * See pot_planner.cpp for the complete example
         */
        struct none {
            none();
            Graph operator()(MapRef map);
        };

        /**
         * \brief The Voronoi roadmap builder.
         *
         * This is used to build a voronoi roadmap of a given map. It is used
         * in the voronoi roadmap based planner. It uses Boost.Polygon library
         * to construct voronoi diagram which is then filtered to remove secondary
         * and other unsafe edges. The diagram is then finally converted to a
         * form suitable for use within MPEL
         */
        struct voronoi {
            /**
             * \param eps Parameter for estimating curves as polygons. It specifies
             * the maximum amount by which the approximation can deviate from the
             * actual curve. A smaller value for eps means a closer approximation but
             * will increase the computation time. A larger value on the other hand
             * will speed up computation but may miss out important structural details
             * of the curve.
             */
            voronoi(double eps = 10);
            Graph operator()(MapRef map);

        private:
            double _eps; // parameter for approximating the workspace
        };

        /**
         * \brief The probabilistic roadmap builder.
         */
        struct probabilistic {
            /**
             * \param n The number of points in the probabilistic roadmap. If n is 0
             * then the number of points are calculated automatically based on a
             * heuristic function. If the number of points specified are unable to form
             * a connected roadmap, the function automatically samples more points to
             * ensure that the roadmap is connected
             */
            probabilistic(size_t n = 0);
            Graph operator()(MapRef map);

        private:
            size_t _n; // number of nodes in the graph 0 => automatically determined
        };
    }

    /// This namespace contains the declarations of builtin graph search algorithms
    namespace graph_search {
        /**
         * \brief The empty graph search algorithm.
         *
         * This algorithm does not actually perform any search and is useful only
         * when debugging application
         */
        struct none {
            none();
            Path operator()(GraphRef g, PointRef a, PointRef b);
        };

        /**
         * \brief The A* heuristic graph search algorithm.
         *
         * This is one of the simplest and most popular search methods used in
         * robot motion planning. The methods searches the graph using a heuristic
         * to determine the node to explore. This method does not guarantee optimality
         * of the path found
         */
        struct a_star {
            a_star();
            Path operator()(GraphRef g, PointRef a, PointRef b);
        };

        /**
         * \brief Dijkstra's shortest path algorithm.
         *
         * This functor implements the Dijkstra's shortest path algorithm on weighted
         * graphs with positive weights. This method takes longer than the A* method
         * but finds the shortest possible path between two points in a given graph
         */
        struct dijkstra {
            dijkstra();
            Path operator()(GraphRef g, PointRef a, PointRef b);
        };

        /**
         * \brief Breadth first search (BFS) algorithm.
         *
         * The BFS method searches the graph and minimizes the number of edges it
         * passes through, this also does not guarantee optimiality of the path
         */
        struct breadth_first {
            breadth_first();
            Path operator()(GraphRef g, PointRef a, PointRef b);
        };

        /**
         * \brief Bi-directional Breadth first search (BFS) algorithm.
         *
         * The Bidirectional-BFS method is similar to the BFS method but works
         * by searching simultaneously from start and goal points, terminating
         * when a point common to both runs is found. It performs better than BFS
         * in most of the cases
         * Reference: http://stackoverflow.com/a/13179222/3033441
         */
        struct bidirectional_breadth_first {
            bidirectional_breadth_first();
            Path operator()(GraphRef g, PointRef a, PointRef b);
        };
    }

    /// This namespace contains all the builtin interpolators aka local planners
    namespace interpolator {

        /**
         * \brief The bug class for building bug based interpolators
         *
         * This class is used in all the bug based interpolators for sensing and
         * moving along obstacles required in all bug based algorithms
         */
        class Bug {
        public:
            Bug(MapRef map, PointRef pos);   ///< Constructs a bug in a map at given location
            void set_goal(PointRef pt);      ///< Sets the goal position of bug
            bool goal_reached();             ///< Returns if the goal position has been reached
            Point position();                ///< current position of the bug
            int obstacle_distance();         ///< Returs the distance to the nearest obstacle
            void follow_wall(int direction); ///< follow the wall in a given direction (+1, -1)
            void move(PointRef dist);        ///< Moves the bug by a specified amount
            void move(int dx, int dy);       ///< Moves the bug by a specified amount
            bool move_to(PointRef pt);       ///< Move to a specified point in a straight line
                                             ///< returns false if the path collides with some obstacle
            bool goal_visible();             ///< Returns if the goal is visible from current location
            Path path();                     ///< Returns the path followed by this bug
        private:
            cv::Mat m_dt; ///< Distance transform of the map
            Path m_path;  ///< Trail of the bug
            Point m_pos;  ///< current position of the bug
            Point m_goal; ///< Goal position of the bug
        };

        /**
         * \brief The empty interpolator.
         *
         * This is used when you want to see the path generated by the global planner
         */
        struct none {
            none();
            Path operator()(MapRef map, PathRef path);
        };

        /**
         * \brief The Bug 2 algorithm
         *
         * \bug The output is erroneous in several cases
         */
        struct bug2 {
            /**
             * \param step The step size by which the bug moves
             */
            bug2(double step = 2);
            Path operator()(MapRef map, PathRef path);

        private:
            double _step;
        };

        /**
         * \brief Interpolator based on artificial potential field
         *
         * This interpolator works by creating an artificial potential field using
         * its knowledge of nearby obstacles and the final goal location. The goal
         * creates an attractive potential given by
         * \f$f_a(\textbf{x}) = c \cdot ||\textbf{x}-\textbf{x}_g||^2\f$
         * and the obstacles create a repulsive potential given by
         * \f$f_r(x)=\eta\cdot(\rho(x)^{-1}-d_0^{-1})^2\f$ for \f$\rho(x)<d_0\f$
         * where \f$\rho(x)\f$ is the distance from the nearest obstacle. The robot
         * then moves in the direction of maximum gradient.
         */
        struct potential_field {
            potential_field();
            Path operator()(MapRef map, PathRef path);
        };
    }

    /// Planner configuration for Voronoi roadmap planner
    struct voronoi_planner_config : Planner::Config {
        voronoi_planner_config();
    };

    /// Planner configuration for PRM planner
    struct PRM_planner_config : Planner::Config {
        PRM_planner_config();
    };
}
}
#endif
