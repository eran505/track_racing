//
// Created by ise on 24.12.2019.
//

#ifndef TRACK_RACING_ASTAR_HPP
#define TRACK_RACING_ASTAR_HPP

#include "util_game.hpp"
#include <vector>
#include <deque>
#include <numeric>
#include <functional>
#include <set>
#include <cmath>
namespace AStar
{
    enum D{d=2};
    struct StatePoint
    {
        Point pos;
        Point speed;
        bool operator == (const StatePoint& coordinates_);
        inline Point & get_speed(){return speed;}
        inline Point & get_position(){return pos;}
        //~StatePoint(){delete (pos);delete (speed);}
        string toStr() const { return pos.to_hash_str()+speed.to_hash_str();}
        StatePoint(const Point& p , const Point& s){
            this->pos=p;
            this->speed=s;
        }
        StatePoint(const StatePoint &other){
            //cout<<"copy StatePoint"<<endl;
            pos=Point(other.pos);
            speed=Point(other.speed);
        }
    };

    using uint = unsigned int;
    using HeuristicFunction = std::function<uint(const StatePoint &a, const StatePoint &b,int maxSpeed)>;
    using CoordinateList = std::vector<StatePoint>;

    struct Node
    {
        double G, H;
        StatePoint *coordinates;
        vector<Node*> parent;

        explicit Node(StatePoint *coord_, Node *parent_);
        explicit Node(StatePoint *coord_);
        ~Node(){//cout<<"~ node"<<endl;
        delete coordinates;}
        Node(const Node &other);
        uint getScore();
        string toStr(){ return coordinates->toStr();}
        //unsigned int hash(int maxSize){ return coordinates->hash()}
    };



    using NodeSet = std::set<Node*>;
    typedef vector<uint> unitVector;
    typedef std::vector<Node*> listNode ;
    typedef unordered_map<u_int64_t, map<int,int>*>  policyDict;
    class Generator
    {

        Node* findNodeOnList(const unordered_map<string,Node*>& nodes_, StatePoint &coordinates_);
        void releaseNodes(NodeSet& nodes_);
        void releaseMAP(unordered_map <string,Node*> map_);

    public:
        unordered_map<u_int64_t ,std::pair<short,StatePoint>> * hashDictStates;
        unordered_map<u_int64_t, map<int,int>*> *dictPoly;
        void print_pathz(Node *l);
        void getDict(unordered_map<u_int64_t,vector<float>*>* dict,double weight=1.0);
        void pathsToDict();
        void pathsToDict_rec(Node &item);
        void getDictPolicy(const listNode &l);
        void setConsistentZ(bool bol){this->consistentZ=bol;}
        Generator(uint absMaxSpeed, Point& girdSize);
        ~Generator(){
            for (auto &itemI:*this->dictPoly)
                delete(itemI.second);
            delete(this->dictPoly);
        }
        void filterPaths();
        void consistentZFilter();
        void setHeuristic(HeuristicFunction heuristic_);
        int findPath( StatePoint& source_, const StatePoint& target,bool toDict = true);
        listNode findComplexPath(StatePoint& source_,Point& mid, const StatePoint& target_);
        int count_pathz(vector<Node*> *l );
        void changeMaxSpeed(uint speedMaxNew){this->absMaxSpeed=speedMaxNew;}
        void setMaxPATH(unsigned long numberMax){maxPath=numberMax;}
        void dictPolyClean() const{
            for (auto &itemI:*this->dictPoly)
                delete(itemI.second);
            this->dictPoly->clear();
        }

        vector<vector<Point>> agentPaths;
    private:
        vector<vector<StatePoint*>> allPath;
        unsigned long maxPath;
        uint absMaxSpeed;
        Point gridSize;
        HeuristicFunction heuristic;
        CoordinateList direction, walls;
        bool consistentZ = true;
        vector<Point> operatorAction;
        list<Node*> listPrint;
        vector<vector<StatePoint>> deepListNode;

        void copyAgentPaths();
        StatePoint* applyActionState(const StatePoint &cur,const Point &action) const{
            auto speed_copy = Point(cur.speed);
            speed_copy+=action;
            speed_copy.change_speed_max(absMaxSpeed);
            auto pos_copy = Point(cur.pos);
            pos_copy+=speed_copy;
            return new StatePoint{pos_copy,speed_copy};
        }
        void printMee(listNode nz){
            for(Node* item: nz){
                //cout<<"------\t------\t------\t------"<<endl;
                print_pathz(item);
            }
        }
        void deepCopyPaths()
        {
            this->deepListNode.clear();
            for (auto &item : allPath){
                vector<StatePoint> listI;
                listI.reserve(item.size());
                for(auto &x : item)
                {
                    listI.push_back(*x);
                }
                this->deepListNode.push_back(listI);
            }
            this->allPath.clear();
        }


        void addToStateDict(u_int64_t key, StatePoint *stateS);
    };

    class Heuristic
    {

    public:
        static uint manhattan(const StatePoint &source_, const StatePoint &target_, int maxSpeed);
        static uint zero(const StatePoint &source_, const StatePoint &target_, int maxSpeed);

    };
}



#endif //TRACK_RACING_ASTAR_HPP
