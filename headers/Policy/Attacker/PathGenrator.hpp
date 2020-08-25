//
// Created by ERANHER on 11.8.2020.
//

#ifndef TRACK_RACING_PATHGENRATOR_HPP
#define TRACK_RACING_PATHGENRATOR_HPP
#include "Policy.hpp"
#include "Policy/Attacker/ABpathFinder.hpp"
#include "Policy/Attacker/normalizer.h"
using AStar::StatePoint;

class PathGenartor{

    Point grid_size;
    ABfinder aBFinder;
    unordered_map<u_int64_t,unordered_map<uint,double>> RAW_policyMap;
    Normalizer nom;
    Randomizer random_gen;
public:
    PathGenartor(u_int64_t seed,const Point &girdSize,u_int32_t max_speed)
    :grid_size(girdSize),aBFinder(seed,girdSize,max_speed),random_gen(seed){

    }
    PathGenartor():grid_size(0),aBFinder(1,grid_size,0),random_gen(1){}


    void add_path(const std::vector<StatePoint> &l,
                  unordered_map<u_int64_t,std::vector<double>*>* mapPolicy,
                  double w=1.0)
    {
        pathsToDict(l);
        nom.getDict(RAW_policyMap,mapPolicy,w);
    }


    void geneate_path_loop(const std::vector<pair<std::vector<Point>,double>> &seq_Goal,const std::vector<weightedPosition> &start_point,u_int num_path,
                           unordered_map<u_int64_t,std::vector<double>*>* mapPolicy)
    {
        for(auto& item_start_point:start_point)
        {
            for(auto& item_end_point:seq_Goal)
            {
                auto seq = make_state_seq(item_end_point.first,item_start_point);
                double w = item_start_point.weightedVal*item_end_point.second;
                inset_path_to_dict(seq,num_path,w,mapPolicy);

            }
        }
        nom.normalizeDict(*mapPolicy);
    }

private:
    static vector<StatePoint> make_state_seq(const std::vector<Point> &l,const weightedPosition &item)
    {
        std::vector<StatePoint> seq;
        seq.emplace_back(item.positionPoint,item.speedPoint);
        for(const auto &itemI:l)
            seq.emplace_back(itemI,Point(0));
        return seq;

    }
    void inset_path_to_dict(const std::vector<StatePoint> &A_list,int num,double w,
                            unordered_map<u_int64_t,std::vector<double>*>* mapPolicy )
    {
        int ctr=0;
        while (true){
            if(ctr==num)
                break;
            add_path_to_dict(A_list);
            ctr++;
        }
        nom.getDict(RAW_policyMap,mapPolicy,w);

    }

    void add_path_to_dict(const std::vector<StatePoint> &A_list)
    {
        vector<AStar::StatePoint> seq_state;
        vector<AStar::StatePoint> seq_state_all;
        auto new_list = add_middle_point_at_random(A_list);
        //auto new_list=A_list;
        for(int k=0;k<new_list.size()-1;++k)
        {
            seq_state = aBFinder.get_pathz(new_list[k],new_list[k+1]);
            for(const auto &x:seq_state)cout<<x.toStr()<<endl;
            std::move(seq_state.begin(), seq_state.end()-1, std::back_inserter(seq_state_all));
        }
        seq_state_all.push_back(seq_state[seq_state.size()-1]);
        pathsToDict(seq_state_all);
    }
    StatePoint get_random_point()
    {
        Point p;
        for(int i=0;i<p.capacity;++i)
            p.array[i]=int(this->random_gen.get_double()*(grid_size[i]*0.75));
        p.array[2]=2;
        p=Point(8,8,1);
        return {p,Point(1,1,0)};
    }
    std::vector<StatePoint> add_middle_point_at_random(const std::vector<StatePoint> &A_list)
    {
        return {*A_list.begin(),get_random_point(),A_list[1]};
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
            //cout<<"ation_h="<<ation_h<<" : "<<difAction.to_hash_str()<<endl;
            //cout<<"key="<<key<<"\t"<<allPath[i].pos.to_hash_str()<<"_"<<allPath[i].speed.to_hash_str()<<endl;
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

#endif //TRACK_RACING_PATHGENRATOR_HPP
