#include <mpel/builtins.hpp>
#include <vector>
#include <queue>
#include "types.hpp"
#include "common.hpp"

namespace mpel {

// default search
default_search::default_search() {}
Path default_search::operator()(GraphRef g, PointRef a, PointRef b) {
	Path path;
	path.push_back(a);
	path.push_back(b);
	return path;
}

// a star search
a_star_search::a_star_search() {
	throw UnimplementedException();
}
Path a_star_search::operator()(GraphRef g, PointRef a, PointRef b) {
	Path path;
	return path;
}

// dijkstra search
dijkstra_search::dijkstra_search() {}
Path dijkstra_search::operator()(GraphRef g, PointRef a, PointRef b) {
	Path path;

	size_t in = g.descriptor(a);
	size_t out = g.descriptor(b);

	std::vector<double> dist(g.num_vertices(), std::numeric_limits<double>::max());
	std::vector<size_t> parent(g.num_vertices());
	dist[in] = 0;
	parent[in] = in;

	while (true) {
		// find the node with min +ve dist
		size_t curr = g.num_vertices();
		double min_dist = std::numeric_limits<double>::max();
		for (size_t i = 0; i < g.num_vertices(); ++i) {
			if (dist[i] < 0) continue;
			if (dist[i] < min_dist) {
				curr = i;
				min_dist = dist[i];
			}
		}

		if (curr == g.num_vertices()) return path; // unable to find path
		if (curr == out) { // reconstruct path
			while (parent[curr] != curr) {
				path.push_back(g.vertex(curr));
				curr = parent[curr];
			}
			path.push_back(g.vertex(curr));
			std::reverse(path.begin(), path.end());
			return path;
		}

		// update cost of all neighbors of curr
		for (size_t i = 0; i < g.num_vertices(); ++i) {
			if (g.weight(curr, i) < 0) continue; // not connected
			double d = dist[curr] + g.weight(curr, i);
			if (d < dist[i]) {
				parent[i] = curr;
				dist[i] = d;
			}
		}

		// delete current node
		dist[curr] = -1;
	}
}

// breadth first search
breadth_first_search::breadth_first_search() {}
Path breadth_first_search::operator()(GraphRef g, PointRef a, PointRef b) {
	Path path;

	size_t in = g.descriptor(a);
	size_t out = g.descriptor(b);

	double max_dist = std::numeric_limits<double>::max();
	std::vector<double> dist(g.num_vertices(), max_dist);
	std::vector<size_t> parent(g.num_vertices());
	std::queue<size_t> Q;
	Q.push(in);

	dist[in] = 0;
	parent[in] = in;

	while (not Q.empty()) {
		size_t curr = Q.front();
		Q.pop();

		for (size_t i = 0; i < g.num_vertices(); ++i) {
			if (g.weight(curr, i) < 0) continue; // not connected
			if (dist[i] == max_dist) {
				dist[i] = dist[curr] + g.weight(curr, i);
				parent[i] = curr;
				Q.push(i);
				// if this is the goal
				if (curr == out) {
					while (parent[curr] != curr) {
						path.push_back(g.vertex(curr));
						curr = parent[curr];
					}
					path.push_back(g.vertex(curr));
					std::reverse(path.begin(), path.end());
					return path;
				}
			}
		}
	}
	return path;
}

// depth_first_search
depth_first_search::depth_first_search() {
	throw UnimplementedException();
}
Path depth_first_search::operator()(GraphRef g, PointRef a, PointRef b) {
	Path path;
	return path;
}
}
