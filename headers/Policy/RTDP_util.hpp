//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDP_UTIL_HPP
#define TRACK_RACING_RTDP_UTIL_HPP
#include <cmath>

#include <stack>
#include "../State.hpp"
#include "../util_game.hpp"
#include <algorithm>

class RTDP_util{
    double **qTable;
    int D = 2;
    map<string,int> *mapState;
    map<int,int> *mapAction;
    map<int,Point*> *int_to_action;
    unsigned int ctr_state=0;
    int ctr_random=0;
    int size_Q;
    int size_mapAction;
    void set_up_action_map(int d);
    void set_up_Q(int grid_size,const list<pair<int,int>>& list_l);
    void heuristic(State *s,int entry_index);

public:
    int last_entry;
    Point get_argmx_action(State *s);
    int get_state_argmax(State *s_state);
    double get_value_state_max(State *s_state);
    ~RTDP_util();
    RTDP_util(int grid_size,const list<pair<int,int>>& max_speed_and_budget);
    int get_state_index_by_string(State *str_state);
    int add_entry_map_state(string &basicString, State *s);
    void set_value_matrix(int entryState, Point &action ,double val){
        int key = action.hash2D();

        auto pos = this->mapAction->find(key);
        if (pos == this->mapAction->end())
            throw;
        else{
            //cout<<"Q("<<entryState<<","<<pos->second<<")="<<this->qTable[entryState][pos->second]<<endl;
            this->qTable[entryState][pos->second]=val;
            //cout<<"Q("<<entryState<<","<<pos->second<<")="<<this->qTable[entryState][pos->second]<<endl;
            }

    }
    vector<float>* get_probabilty(State *s);
};


#endif //TRACK_RACING_RTDP_UTIL_HPP
