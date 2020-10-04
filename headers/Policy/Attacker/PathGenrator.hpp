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
    Randomizer random_gen;
public:
    PathGenartor(u_int64_t seed,const Point &girdSize,u_int32_t max_speed)
    :grid_size(girdSize),aBFinder(seed,girdSize,max_speed),random_gen(seed){

    }
    PathGenartor():grid_size(0),aBFinder(1,grid_size,0),random_gen(1){}





    pair<std::vector<double>,std::vector<std::vector<StatePoint>>> geneate_path_loop
    (const std::vector<pair<std::vector<Point>,double>> &seq_Goal,const std::vector<weightedPosition> &start_point,uint32_t num_paths)
    {
        std::vector<double> list_prob;
        std::vector<std::vector<StatePoint>> list_pathz;
        list_prob.reserve(start_point.size()*seq_Goal.size()*num_paths);
        list_pathz.reserve(start_point.size()*seq_Goal.size()*num_paths);

        for(auto& item_start_point:start_point)
        {
            for(auto& item_end_point:seq_Goal)
            {
                for(int i=0;i<num_paths;++i)
                {
                    auto seq = make_state_seq(item_end_point.first,item_start_point);
                    double w = item_start_point.weightedVal*item_end_point.second;
                    auto pathI = generate_path_candidate(seq);
                    list_pathz.push_back(pathI);
                    list_prob.push_back(w);
                }

            }
        }
        return {list_prob,list_pathz};
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


    vector<AStar::StatePoint> generate_path_candidate(const std::vector<StatePoint> &A_list)
    {
        vector<AStar::StatePoint> seq_state;
        vector<AStar::StatePoint> seq_state_all;
        //auto new_list = add_middle_point_at_random(A_list);
        auto new_list=A_list;
        for(int k=0;k<new_list.size()-1;++k)
        {
            seq_state = aBFinder.get_pathz(new_list[k],new_list[k+1]);
            std::move(seq_state.begin(), seq_state.end()-1, std::back_inserter(seq_state_all));
        }
        seq_state_all.push_back(seq_state[seq_state.size()-1]);
        return seq_state_all;
    }
    StatePoint get_random_point(const StatePoint& sP)
    {
        Point p;
        for(int i=0;i<p.capacity-1;++i)
            p.array[i]=int(this->random_gen.get_double()*(sP.pos[i]*0.9));
        p.array[2]=int(this->random_gen.get_double()*(3.0));

        return {p,Point(0,0,0)};
    }
    std::vector<StatePoint> add_middle_point_at_random(const std::vector<StatePoint> &A_list)
    {
        return {*A_list.begin(),get_random_point(A_list.back()),A_list[1]};
    }

};

#endif //TRACK_RACING_PATHGENRATOR_HPP
