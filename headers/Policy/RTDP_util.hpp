//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDP_UTIL_HPP
#define TRACK_RACING_RTDP_UTIL_HPP
#include <cmath>
#include "../util/csvfile.hpp"
#include <stack>
#include "../State.hpp"
#include "../util_game.hpp"
#include <fstream>
#include <algorithm>
#include "Policy.hpp"
class RTDP_util{
    double **qTable;
    map<string,int> *mapState;
    vector<Policy*> *lTran= nullptr;
    const string home;
    Policy* my_policy;
    unsigned int ctr_state=0;
    int ctr_random=0;
    int size_Q;
    int size_mapAction;
    unordered_map<int,Point*>* hashActionMap;
    double collReward=1;double goalReward=-1;double wallReward=-1;
    void set_up_Q(int grid_size,vector<pair<int,int>>& list_l);
    void heuristic(State *s,int entry_index);
    double compute_h(State *s);

public:
    void policyData();
    bool apply_action(State *s,const string &id,Point &action,int max_speed);
    void set_tran(vector<Policy*>* l){this->lTran=l;}
    void MyPolicy(Policy *my){this->my_policy=my;}
    double discountFactor=0.988;
    int last_entry;
    Point get_argmx_action(State *s);
    int get_state_argmax(State *s_state);
    double get_value_state_max(State *s_state);
    ~RTDP_util();
    RTDP_util(int grid_size,vector<pair<int,int>>& max_speed_and_budget,string &mHome);
    int get_state_index_by_string(State *str_state);
    unsigned int add_entry_map_state(string &basicString, State *s);

    void set_value_matrix(int entryState, Point &action ,double val){
//        if (entryState == 0)
//            cout<<action.hashMeAction(Point::actionMax)<<endl;
//        if (action.hashMeAction(Point::actionMax)>26)
//            cout<<"bigger"<<endl;
        //cout<<"Q("<<entryState<<","<<action.hashMeAction(Point::actionMax)<<")="<<this->qTable[entryState][action.hashMeAction(Point::actionMax)]<<endl;
        this->qTable[entryState][action.hashMeAction(Point::actionMax)]=val;
        //cout<<"Q("<<entryState<<","<<action.hashMeAction(Point::actionMax)<<")="<<this->qTable[entryState][action.hashMeAction(Point::actionMax)]<<endl;


    }
    vector<float>* get_probabilty(State *s);

    double rec_h(State *s, int index,double acc_probablity);

};


#endif //TRACK_RACING_RTDP_UTIL_HPP
