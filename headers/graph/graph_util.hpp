//
// Created by ise on 14.12.2019.
//

#ifndef TRACK_RACING_GRAPH_UTIL_HPP
#define TRACK_RACING_GRAPH_UTIL_HPP
//
//#include <boost/graph/grid_graph.hpp>
//#include <boost/graph/graphviz.hpp>
//#include <boost/array.hpp>
//#include <cstddef>
//#include "util_game.hpp"
//#include <boost/graph/graph_utility.hpp> // print_graph
//
////
//using namespace boost;
//typedef grid_graph<2> Graph;
//typedef graph_traits<Graph> Traits;
//typedef list<Point*> list_point;
//
//
//
//class GraphUtil{
//
//    Graph *g_graph;
//public:
//    void print_me(){print_graph(*g_graph);}
//    void GraphUtil_ShortestPath(size_t x,size_t y);
//    static void print_vertex_my(Traits::vertex_descriptor vertex_to_print) {
//        std::cout << "(" << vertex_to_print[0] << ", " << vertex_to_print[1] << ")" << std::endl;
//    }
//
//};
//
////###########################cpp#########################################
//
//void GraphUtil::GraphUtil_ShortestPath(size_t x, size_t y) {
//
//    boost::array<size_t, 2> lengths = { { x, y } };
//    Graph graph(lengths);
//    g_graph = &graph;
//    for (Traits::vertices_size_type v_index = 0;
//         v_index < num_vertices(graph); ++v_index) {
//
//        // The two indicies should always be equal
//        std::cout << "Index of vertex " << v_index << " is " <<
//                  get(boost::vertex_index, graph, vertex(v_index, graph)) << std::endl;
//
//    }
//    // Do a round-trip test of the edge index functions
//    for (Traits::edges_size_type e_index = 0;
//         e_index < num_edges(graph); ++e_index) {
//
//        // The two indicies should always be equal
//        std::cout << "Index of edge " << e_index << " is " <<
//                  get(boost::edge_index, graph, edge_at(e_index, graph)) << std::endl;
//    }
//
//
//    // Print number of dimensions
//    std::cout << graph.dimensions() << std::endl;
//
//    // Print dimension lengths (same order as in the lengths array)
//    std::cout << graph.length(0) << "x" << graph.length(1) << std::endl; // prints "3x5x7"
//    cout<<"----------"<<endl;
//    Traits::vertex_descriptor first_vertex = vertex(3, graph);
//    Traits::vertex_descriptor goal_vertex = vertex(0, graph);
//
//    print_graph(graph, std::cout);
//
//}



#endif //TRACK_RACING_GRAPH_UTIL_HPP
