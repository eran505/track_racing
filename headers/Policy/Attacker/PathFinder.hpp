//
// Created by ERANHER on 30.7.2020.
//

#ifndef TRACK_RACING_PATHFINDER_HPP
#define TRACK_RACING_PATHFINDER_HPP

#include <utility>

#include "Policy.hpp"
#include "Policy/Attacker/ABpathFinder.hpp"
#include "Policy/Attacker/normalizer.h"
#include "PathGenrator.hpp"
using AStar::StatePoint;

class PathFinder: public Policy{

    vector<weightedPosition> starting_point;
    std::unique_ptr<unordered_map<u_int64_t,std::vector<double>*>>  policyMap;
    PathGenartor genartor_path;

public:
    PathFinder(string namePolicy,int speed_MAX,const string &agentID,string &home,
            std::vector<pair<std::vector<Point>,double>> &seq_Goal,const std::vector<weightedPosition>& start_point,const Point &GridSize,u_int64_t seed,int num_paths,double stcho=1.0)
    : Policy(std::move(namePolicy),speed_MAX,agentID,home)
    , starting_point(start_point)
    , policyMap(std::make_unique<unordered_map<u_int64_t,std::vector<double>*>>())
    , genartor_path(seed,GridSize,max_speed)
    {
        genartor_path.geneate_path_loop(seq_Goal,start_point,num_paths,this->policyMap.get());
        cout<<"[Attacker] END generation"<<endl;
    }

    PathFinder(int speed_MAX,const string &agentID,string &home,const double p,const vector<StatePoint> &lPath)
    :Policy("PathFinder",speed_MAX,agentID,home)
    ,policyMap(std::make_unique<unordered_map<u_int64_t,std::vector<double>*>>())
    {
        this->genartor_path.add_path(lPath,policyMap.get());
    }

    Point get_action(State *s) override
    {
        auto EntryIndx = get_hash_state(s);
        std::unordered_map<u_int64_t , std::vector<double>*>::iterator it;
        it = this->policyMap->find(EntryIndx);
        if (it==this->policyMap->end())
            throw;
        // get value between zero to one
        double r = this->getRandom();
        double acc=0;
        size_t i;
        for (i=1; i<it->second->size() ; ++i) {
            acc+=it->second->operator[](i);
            if (r<=acc)
                break;
            i+=1;
        }
        // problem with precision number (doubleing point)
        if (it->second->size()<i)
        {
            i=it->second->size()-1;
        }

        int indexI =int(it->second->operator[](int(i-1)));
        Point choosenAction = *this->hashActionMap->at(indexI);
        return choosenAction;

    }


    void reset_policy() override {}
    void policy_data()const override {}
    const vector<double >* TransitionAction(const State *s)const override {
        /* first  - action
         * second - probabilitiy*/
        //cout<<"[debug] "<<s->to_string_state()<<endl;
        auto EntryIdx = get_hash_state(s);
        auto pos = this->policyMap->find(EntryIdx);
//        if (pos->second->size()!=2)
//            for(const auto &item:*pos->second) cout<<item<<endl;
        if (pos==this->policyMap->end())
            throw std::runtime_error(std::string("Error: cant find the key "));
        return this->policyMap->at(EntryIdx);
    }
    u_int64_t get_hash_state(const State *s)const {
        //cout<<s->to_string_state()<<endl;
        auto hSpeed = s->get_speed(this->id_agent).hashConst(Point::maxSpeed);
        auto hPos = s->get_position_ref(this->id_agent).hashConst();
        u_int64_t EntryIndx = Point::hashNnN(hPos,hSpeed);
        return EntryIndx;
    }



    void treeTraversal(State *ptrState, vector<pair<double,vector<StatePoint>>>* myPaths= nullptr)const
    {

        std::deque<pair<State,double>> q;
        q.emplace_back(*ptrState,1);
        double probAcc=1;
        vector <pair<State,double>> path;
        while (!q.empty())
        {
            auto pairCur = q.front();
            auto curState = std::get<0>(pairCur);
            auto prob =std::get<1>(pairCur);

            //cout<<curState.to_string_state()<<endl;
            q.pop_front();
            if(curState.isEndState(this->id_agent))
            {
                vector<StatePoint> v;
                path.emplace_back(curState,prob);
                //v.push_back(std::to_string(probAcc));
                for( auto& item : path)
                {
                    auto posA = item.first.get_position_ref(this->id_agent);
                    v.emplace_back(posA,item.first.get_speed_ref(this->id_agent));
                    //cout<<posA.to_str()<<",";
                }
                //cout<<endl;
                auto posPair = path[path.size()-1];
                auto p = posPair.second;
                probAcc = probAcc/p;
                path.pop_back();
                myPaths->emplace_back(probAcc,std::move(v));
                continue;
            }
            if (!path.empty())
                if (curState.to_string_state() == std::get<0>( path[path.size()-1]).to_string_state())
                {
                    auto posPair = path[path.size()-1];
                    auto p = posPair.second;
                    probAcc = probAcc/p;
                    path.pop_back();
                    continue;
                }
            path.emplace_back(curState,prob);
            auto next =Normalizer::minizTrans(this->TransitionAction(&curState));
            q.push_front({curState,probAcc});
            probAcc=probAcc*prob;
            for (int i = 0 ; i<next.size() ; ++i)
            {
                auto pos = this->hashActionMap->find(next.operator[](i));
                if ( pos == this->hashActionMap->end())
                    throw std::invalid_argument("Action index is invalid");
                Point *actionI = pos->second;
                State tmp(curState);
                tmp.applyAction(this->id_agent,*actionI,this->max_speed);
                //cout<<tmp.to_string_state()<<endl;
                q.push_front({tmp,next[++i]});
            }

        }
        cout<<"[treeTraversal]"<<endl;
    }



};

#endif //TRACK_RACING_PATHFINDER_HPP
