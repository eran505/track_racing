//
// Created by ERANHER on 30.7.2020.
//

#ifndef TRACK_RACING_ABPATHFINDER_HPP
#define TRACK_RACING_ABPATHFINDER_HPP
#include "util_game.hpp"
#include "serach/Astar.hpp"
#include "util/Rand.hpp"
using AStar::StateSearch;

class ABfinder{
    Randomizer randomizer_obj;
    Point GridSzie;
    double stho=0.9;
    u_int limt=8;
    u_int16_t MAX_SPEED=2;
    Point last_action;
    vector<Point> seq_action;
    ABfinder(u_int64_t seed,const Point &grid_size,int max_seed):
    randomizer_obj(seed),
    GridSzie(grid_size)
    {}
    void get_path(const StateSearch &A,const StateSearch &B)
    {
        StateSearch cur = A;
        while(less_than_limit(cur,B))
        {
            bool bol=true;
            while(bol)
            {
                get_action_to_goal(cur,B);
                bol=!vaild_move(cur);
            }
            seq_action.emplace_back(last_action);
        }
    }
    void get_action_to_goal(StateSearch & cur,const StateSearch& Goal)
    {

        if(inset_noise()) return;
        move_to_goal(cur,Goal);
    }
    [[nodiscard]] bool less_than_limit(const StateSearch& cur,const StateSearch& Goal)const
    {
        if(Goal.pos>cur.pos+limt)
            return true;
        return false;
    }
    bool inset_noise()
    {
        if(randomizer_obj.get_double()>stho)
        {
            for(int i=0;i<last_action.capacity;++i)
                last_action.array[i]=get_move_aixs_random(randomizer_obj.get_double());
            return true;
        }
        return false;
    }
    void move_to_goal(StateSearch & cur,const StateSearch& Goal)
    {
        for(int k=0;k<last_action.capacity;++k)
            last_action.array[k]=get_move_aixs(k,cur,Goal);

    }
    [[nodiscard]] static int get_move_aixs(int i,const StateSearch &cur,const StateSearch& Goal)
    {

        auto m = Goal.pos[i] - (cur.pos[i]+cur.speed[i]);
        if (m>1)
            m=1;
        else if (m<-1)
            m=-1;
        return m;
    }
    void apply_action_StateSearch(const Point& action, StateSearch &cur) const
    {
        cur.speed+=action;
        cur.speed.change_speed_max(this->MAX_SPEED);
        cur.pos+=cur.speed;
    }
    bool vaild_move(StateSearch &cur)const
    {
        StateSearch cur_old(cur);
        apply_action_StateSearch(last_action,cur);
        if(is_out_bound(cur)){
            cur=cur_old;
            return false;
        }
        return true;
    }
    [[nodiscard]] bool is_out_bound(const StateSearch &cur)const{
        if(cur.pos>=this->GridSzie) return true;
        if(cur.pos+cur.speed>=this->GridSzie+1) return true;
        return false;
    }
    static int get_move_aixs_random(double d)
    {
        if(d<0.33)
            return -1;
        if(d<0.66)
            return 0;
        else
            return 1;
    }

};

#endif //TRACK_RACING_ABPATHFINDER_HPP
