#include <mpel/core.hpp>
#include <mpel/builtins.hpp>
#include <iostream>

using namespace mpel;

int main(int argc, char **argv) {

	if (argc < 2) {
		std::cerr << "Demo requires a map image filename" << std::endl;
		return -1;
	}

	Planner::Config pc;
	pc.graph_builder = voronoi_graph_builder();
	pc.graph_search = dijkstra_search();
	pc.interpolator = default_interpolator();

	Workspace ws;
	ws.map = load_map_from_image(argv[1]);

	Planner p(pc);
	p.load_workspace(ws);

	ProblemDefinition pdef;
	pdef.start = Point(34, 23);
	pdef.goal = Point(340, 213);

	Path path = p.solve(pdef);
	std::cout << "Length of path: " << path.size() << std::endl;
	for (auto it = path.begin(); it != path.end(); ++it)
		printf("[%d  %d]  ", it->x, it->y);
	std::cout << std::endl;

	View v;
	v.add_layer(ws.map);
	v.add_layer(pdef);
	v.add_layer(p.roadmap());
	v.add_layer(path);
	View::stay();

	return 0;
}

