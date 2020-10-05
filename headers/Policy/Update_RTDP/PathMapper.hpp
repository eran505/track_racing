//
// Created by eranhe on 01/10/2020.
//

#ifndef TRACK_RACING_PATHMAPPER_HPP
#define TRACK_RACING_PATHMAPPER_HPP

#include <utility>

#include "State.hpp"


template <typename K>
class PathMapper{
    std::vector<std::vector<StatePoint>> all_paths;
    unsigned int size_pathz=0;
    std::vector<double> probabilities;
    u_int64_t time_t = 0;
    u_int16_t current_path=-1;
    u_int32_t step_counter=0;
    std::unordered_map<uint64_t,vector<K>> mapper_pathz;
    std::vector<u_int32_t> memo;
public:
    explicit PathMapper(vector<vector<StatePoint>> pathz,std::vector<double> probabilities_paths)
    :all_paths(std::move(pathz)),size_pathz(all_paths.size())
    ,probabilities(std::move(probabilities_paths)){
        crate_maper();
        int max = all_paths[0].size();
        for(int i=1;i<size_pathz;++i)
        {
            if (max<all_paths[i].size()){
                max=all_paths[i].size();
            }

        }
        memo=std::vector<u_int32_t>(max);

    }

    PathMapper()=default;
    void random_choose_path(double seed)
    {
        assert(step_counter==0);
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
        step_counter=0;
        this->memo[step_counter]=time_t;
    }
    vector<pair<StatePoint,double>> get_next_states(u_int64_t hash_state,int jumps)
    {
        assert(step_counter>=0);
        time_t=memo[step_counter--];
        //cout<<"step_counter:\t"<<step_counter<<endl;
        std::vector<K> indexing_arr = this->mapper_pathz.at(hash_state);
        vector<pair<StatePoint,double>> next_states_list;
        next_states_list.reserve(indexing_arr.size());
        std::for_each(indexing_arr.begin(),indexing_arr.end(),[&](const K& index_path){
            next_states_list.emplace_back(get_jumping_state(time_t,index_path),this->probabilities[index_path]);
        });
        for(const auto& item:next_states_list)
        {
            //cout<<"p:"<<item.second<<" s':"<<item.first<<"\t";
        }

        return next_states_list;

    }
    StatePoint get_next_actual_state(u_int jump=1)
    {
        assert(jump>0);
        time_t+=jump;
        step_counter++;
        //cout<<"step_counter"<<step_counter<<endl;
        memo[step_counter]=time_t;

        return get_jumping_state(time_t,this->current_path);
    }
    [[nodiscard]] std::vector<std::vector<StatePoint>> get_all_pathz()const{return all_paths;}
    [[nodiscard]] std::vector<double> get_all_probabilites()const{return probabilities;}
    [[nodiscard]] std::vector<std::vector<Point>> get_all_pos()const
    {
        std::vector<std::vector<Point>> l;
        l.reserve(all_paths.size());
        for(const auto &item:all_paths)
        {
            std::vector<Point> li;
            li.reserve(item.size());
            std::transform(item.begin(), item.end(), std::back_inserter(li), [](const StatePoint& s){
                return s.pos;
            });
            l.push_back(std::move(li));
        }
        return l;
    }


private:

    void crate_maper()
    {
        std::unordered_map<uint64_t,vector<K>> mapper;
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
        for(const auto& item:all_paths)
            for(int i=0;i<item.size();++i)
                cout<<"["<<i<<"] "<<item[i]<<endl;
            cout<<"\n\n"<<endl;

    }

    StatePoint get_jumping_state(u_int64_t idx_time,size_t idx_path)
    {
        auto t = std::min(idx_time,this->all_paths[idx_path].size()-1);
        return this->all_paths[idx_path][t];
    }




};
#endif //TRACK_RACING_PATHMAPPER_HPP
