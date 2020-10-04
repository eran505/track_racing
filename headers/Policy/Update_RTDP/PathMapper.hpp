//
// Created by eranhe on 01/10/2020.
//

#ifndef TRACK_RACING_PATHMAPPER_HPP
#define TRACK_RACING_PATHMAPPER_HPP

#endif //TRACK_RACING_PATHMAPPER_HPP
#include <utility>

#include "State.hpp"
#include "Policy/Attacker/PathFinder.hpp"


template <typename T=u_int16_t>
class PathMapper{
    std::vector<std::vector<StatePoint>> all_paths;
    const uint32_t size_pathz;
    std::vector<double> probabilities;
    u_int64_t time_t = 0;
    u_int16_t current_path=-1;
    u_int32_t step_t=0;
    std::unordered_map<uint64_t,vector<T>> mapper_pathz;
    std::vector<u_int64_t> memo;
public:
    explicit PathMapper(const vector<vector<StatePoint>> & pathz,std::vector<double> probabilities_paths):all_paths(pathz),size_pathz(pathz.size())
    ,probabilities(std::move(probabilities_paths)){
        crate_maper();
        int max = all_paths[0].size();
        for(int i=1;i<size_pathz;++i)
        {
            if (max<all_paths[i].size()){
                max=all_paths[i].size();
            }

        }
    }
    void random_choose_path(double seed)
    {
        double acc=0;
        u_int i=0;
        while (i<(size_pathz-1))
        {
            acc+=this->probabilities[i];
            if(acc>=seed)
                break;
            ++i;
        }
        this->current_path=i;
        this->time_t=0;
        this->step_t=0;
        this->memo[step_t]=time_t;
    }
    vector<pair<StatePoint,double>> get_next_states(u_int64_t hash_state,int jumps)
    {
        assert(jumps>0);
        time_t+=jumps;
        std::vector<T> indexing_arr = this->mapper_pathz.at(hash_state);
        vector<pair<StatePoint,double>> next_states_list;
        next_states_list.reserve(indexing_arr.size());
        std::for_each(indexing_arr.begin(),indexing_arr.end(),[&](T index_path){
            next_states_list.emplace_back(get_jumping_state(time_t,index_path),this->probabilities[index_path]);
        });
        return next_states_list;

    }
    StatePoint get_next_actual_state(u_int jump=1)
    {
        assert(jump>0);
        time_t+=jump;
        memo[step_t]=time_t;
        return get_jumping_state(time_t,this->current_path);
    }
private:

    void crate_maper()
    {
        std::unordered_map<uint64_t,vector<T>> mapper;
        for(int i=0;i<size_pathz;++i)
        {
            for (const auto &item:all_paths[i])
            {
                u_int64_t h= item.getHashStateAttacker();
                if(auto pos = mapper_pathz.find(h);pos==mapper_pathz.end())
                {
                    auto itesr = mapper_pathz.try_emplace(h,1);
                    itesr.first->second[0]=i;
                }
                else{
                    pos->second.push_back(i);
                }
            }
        }
        for(const auto& item:mapper_pathz)
            cout<<""<<item.first<<":"<<item.second<<"\n";

    }

    StatePoint get_jumping_state(u_int64_t idx_time,size_t idx_path)
    {
        auto t = std::min(idx_path,this->all_paths[idx_path].size()-1);
        return this->all_paths[idx_path][t];
    }




};