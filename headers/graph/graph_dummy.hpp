//
// Created by ise on 21.12.2019.
//
//
#ifndef TRACK_RACING_GRAPH_DUMMY_HPP
#define TRACK_RACING_GRAPH_DUMMY_HPP
#include "util_game.hpp"

using std::vector;
using std::map;
using std::unordered_map;
using std::cout;
using std::list;
using std::deque;
using std::endl;
// data structure to store graph edges
struct Edge {
    int src, dest;
};



// class to represent a graph object
typedef vector<vector<int>> listVector;
class Graph_dummy
{
public:
    // construct a vector of vectors to represent an adjacency list
    vector<vector<int>> adjList;
    int size;
    unsigned int path_number=0;
    int ** dist_matrix;
    int xAxis;
    listVector mapMove;
    int yAxis;
    // Graph Constructor
    Graph_dummy(int x,int y)
    {
        // resize the vector to N elements of type vector<int>
        int N = x*y;
        xAxis=x;
        yAxis=y;
        size=N;
        adjList.resize(N);
        for (int i = 0; i < x ; ++i) {
            for (int j = 0; j < y; ++j) {
                if (i - 1 >= 0)
                    this->add_edge({i * x + j, (i - 1) * x + j});
                if (j - 1 >= 0)
                    this->add_edge({i * x + (j), i * x + (j - 1)});


            }
        }
        *dist_matrix = new int[this->size];
        for (int k = 0; k < this->size; ++k) {
            dist_matrix[k]=new int[this->size];
        }
        // init the map graph move
        this->mapMove.reserve(N);

    }
    int  pointToNodeId(Point &xy){
        return xy[0]*xAxis+xy[1];
    }
    void add_edge(Edge edge){
        adjList[edge.src].push_back(edge.dest);
        adjList[edge.dest].push_back(edge.src);
    }
    void printGraph()
    {
        for (int i = 0; i < this->size; i++)
        {
            // print current vertex number
            cout << i << " --> ";

            // print all neighboring vertices of vertex i
            for (int v : adjList[i])
                cout << v << " ";
            cout << endl;
        }
    }
    void BFS(int s)
    {
        // Mark all the vertices as not visited
        bool *visited = new bool[this->size];
        int *dist = new int[this->size];

        for(int i = 0; i < this->size; i++)
            visited[i] = false;

        // Create a queue for BFS
        list<int> queue;

        // Mark the current node as visited and enqueue it
        visited[s] = true;
        queue.push_back(s);
        dist[s]=0;
        // 'i' will be used to get all adjacent
        // vertices of a vertex
        list<int>::iterator i;

        while(!queue.empty())
        {
            // Dequeue a vertex from queue and print it
            auto s_i = queue.front();
            cout << s_i << " ";
            queue.pop_front();

            // Get all adjacent vertices of the dequeued
            // vertex s. If a adjacent has not been visited,
            // then mark it visited and enqueue it
            for(auto &i: this->adjList[s_i])
            {
                if (!visited[i])
                {
                    visited[i] = true;
                    dist[i]=dist[s_i]+1;
                    queue.push_back(i);
                }
            }
        }
        for (int j = 0; j < this->size; ++j) {
            cout<<'['<<j<<']'<<'='<<dist[j]<<endl;
        }
        dist_matrix[s]=dist;
    }

    void printpath(vector<int>& path)
    {
        this->path_number++;
//        for (int i = 0; i < size; i++)
//            cout << path[i] << " ";
//        cout << endl;
    }

// utility function to check if current
// vertex is already present in path
    int isNotVisited(int x, vector<int> path)
    {
        int size = path.size();
        for (int i = 0; i < size; i++)
            if (path[i] == x)
                return 0;
        return 1;
    }
    void shortestPathK(int src,int dst,int k,vector<vector<int>> &all_paths){
        this->BFS(src);
        cout<<this->dist_matrix[src][dst]<<endl;
        this->findpaths(src,dst,k+this->dist_matrix[src][dst],all_paths);
        this->insetToMoveMap(all_paths);
    }

    void insetToMoveMap(const listVector& pathz){
        for( const auto &item : pathz)
        {
            for (unsigned long i = 0; i < item.size()-1; ++i)
                this->mapMove[item[i]].push_back(item[i+1]);
        }
    }
// utility function for finding paths in graph
// from source to destination
    void findpaths( int src,
                   int dst,int k,vector<vector<int>> &all_paths)
    {
        this->path_number=0;
        // create a queue which stores
        // the paths
        deque<vector<int>> q;

        // path vector to store the current path
        vector<int> path;
        path.push_back(src);
        q.push_back(path);
        while (!q.empty()) {
            path = q.front();
            q.pop_front();
            if (path.size()>k+1)
                continue;

            int last = path[path.size() - 1];

            // if last vertex is the desired destination
            // then print the path
            if (last == dst)
            {
                printpath(path);
                all_paths.push_back(path);
            }



            // traverse to all the nodes connected to
            // current vertex and push new path to queue
            for (int i : this->adjList[last]) {
                if (isNotVisited(i, path)) {
                    vector<int> newpath(path);
                    newpath.push_back(i);
                    q.push_front(newpath);
                }
            }
        }
        cout<<"NumPath: "<<this->path_number<<endl;
    }

};

// print adjacency list representation of graph



#endif //TRACK_RACING_GRAPH_DUMMY_HPP
