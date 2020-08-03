//
// Created by ERANHER on 30.7.2020.
//

#ifndef TRACK_RACING_PATHFINDER_HPP
#define TRACK_RACING_PATHFINDER_HPP

#include <utility>

#include "Policy.hpp"
#include "Policy/Attacker/ABpathFinder.hpp"
#include "Policy/Attacker/normalizer.h"
using AStar::StatePoint;

class PathFinder: public Policy{
    double _stcho;
    Point grid_size;
    vector<weightedPosition> starting_point;
    ABfinder aBFinder;
    unordered_map<u_int64_t,unordered_map<uint,double>> RAW_policyMap;
    std::unique_ptr<unordered_map<u_int64_t,std::vector<double>*>>  policyMap;
    Normalizer nom;
public:
    PathFinder(string namePolicy,int speed_MAX,const string &agentID,string &home,const dictionary &ptrDict,
            std::vector<pair<std::vector<Point>,double>> &seq_Goal,std::vector<weightedPosition> start_point,const Point &GridSize,u_int64_t seed,int num_paths,double stcho=1.0)
    : Policy(std::move(namePolicy),speed_MAX,agentID,home, ptrDict)
    , _stcho(stcho)
    , grid_size(GridSize)
    , starting_point(start_point)
    , aBFinder(seed, grid_size, this->max_speed)
    , policyMap(std::make_unique<unordered_map<u_int64_t,std::vector<double>*>>())
    {
        geneate_path_loop(seq_Goal,start_point,num_paths);
        cout<<"[Attacker] END generation"<<endl;
    }
    void geneate_path_loop(const std::vector<pair<std::vector<Point>,double>> &seq_Goal,const std::vector<weightedPosition> &start_point,u_int num_path)
    {
        for(auto& item_start_point:start_point)
        {
            for(auto& item_end_point:seq_Goal)
            {
                auto seq = make_state_seq(item_end_point.first,item_start_point);
                double w = item_start_point.weightedVal*item_end_point.second;
                inset_path_to_dict(seq,num_path,w);

            }
        }
        nom.normalizeDict(*this->policyMap);
    }
    u_int32_t getNumberOfState(){ return policyMap->size();}


    void inset_path_to_dict(const std::vector<StatePoint> &A_list,int num,double w)
    {
        int ctr=0;
        while (true){
            if(ctr==num)
                break;
            add_path_to_dict(A_list);
            ctr++;
        }
        nom.getDict(RAW_policyMap,this->policyMap.get(),w);

    }

    void add_path_to_dict(const std::vector<StatePoint> &A_list)
    {
        vector<AStar::StatePoint> seq_state_all;
        for(int k=0;k<A_list.size()-1;++k)
        {
            vector<AStar::StatePoint> seq_state = aBFinder.get_pathz(A_list[k],A_list[k+1]);
            std::move(seq_state.begin(), seq_state.end(), std::back_inserter(seq_state_all));
        }
        pathsToDict(seq_state_all);
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
    static vector<StatePoint> make_state_seq(const std::vector<Point> &l,const weightedPosition &item)
    {
        std::vector<StatePoint> seq;
        seq.emplace_back(item.positionPoint,item.speedPoint);
        for(const auto &itemI:l)
            seq.emplace_back(itemI,Point(0));
        return seq;

    }

    void reset_policy() override {}
    void policy_data()const override {}
    const vector<double >* TransitionAction(const State *s) override {
        /* first  - action
         * second - probabilitiy*/

        auto EntryIdx = get_hash_state(s);
        auto pos = this->policyMap->find(EntryIdx);
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

    void pathsToDict(const vector<AStar::StatePoint>& allPath) {
        for (unsigned long i = 0; i < allPath.size()-1; ++i) {
            Point difAction = allPath[i+1].speed.operator-(allPath[i].speed);

            u_int64_t key = Point::hashNnN(allPath[i].pos.hashConst(),
                                           allPath[i].speed.hashConst(Point::maxSpeed));
            if (key<u_int64_t(0))
                cout<<endl;
            if (i==0)
                cout<<allPath[i].pos.to_str()<<endl;
            cout<<allPath[i+1].pos.to_str()<<endl;

            u_int ation_h = difAction.hashMeAction(Point::D_point::actionMax);
            auto pos = RAW_policyMap.find(key);
            if (pos == RAW_policyMap.end()) {
                RAW_policyMap.try_emplace(key);
            }
            pos = RAW_policyMap.find(key);
            auto posSec = pos->second.find(ation_h);
            if (posSec == pos->second.end()) {
                pos->second.insert({ation_h, 1});
            } else {
                posSec->second++;
            }
        }

    }



};

#endif //TRACK_RACING_PATHFINDER_HPP
