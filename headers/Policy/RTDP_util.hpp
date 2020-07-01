//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDP_UTIL_HPP
#define TRACK_RACING_RTDP_UTIL_HPP
#include <cmath>
#include <functional>
#include "../util/csvfile.hpp"
#include <stack>
#include "../State.hpp"
#include "../util_game.hpp"
#include <fstream>
#include <algorithm>
#include <utility>
#include "Policy.hpp"
#define VECTOR


typedef u_int64_t keyItem;
typedef double cell;

class RTDP_util{

    enum {sizeN=27};


    #ifdef VECTOR
    typedef std::vector<cell> arr;
    #else
    typedef std::array<cell,sizeN> arr;
    #endif
    std::unique_ptr<unordered_map<keyItem ,arr>>  qTable=
            std::make_unique<unordered_map<keyItem ,arr>>();
    double _stochasticMovement=1;
    u_int64_t update_counter=0;
    vector<Policy*> *lTran= nullptr;
    unordered_map<keyItem,string> debugDict;
    const string home;
    std::function<u_int64_t (const State*)> HashFuction;
    Policy* my_policy= nullptr;
    unsigned int ctr_state=0;
    size_t ctr_random=0;
    double epslion=0.000000001;
    int size_Q;
    int size_mapAction;
    unordered_map<int,Point*>* hashActionMap;
    double collReward=1;double goalReward=-1;double wallReward=-10;
    void heuristic(const State *s,keyItem entry_index);
    cell compute_h(State *s);

    keyItem getStateKeyValue(const State *s) const
    {
        return HashFuction(s);
    }

    arr& get_Q_entry_values(const State *s,keyItem key)
    {
        if(auto pos = qTable->find(key);pos==qTable->end())
        {
            add_entry_map_state(key,s);
            return qTable->operator[](key);
        }
        else return pos->second;
    }
    double applyNonAction(const State *s);

public:
    u_int64_t get_update_ctr() const{return this->update_counter;}
    bool isInQ(const State *s) const
    {
        if(auto pos = this->qTable->find(getStateKeyValue(s));pos==qTable->end())
            return false;
        return true;
    }
    void printInfoGen()
    {
        cout<<"SizeQ:"<<size_Q<<"\tgen: "<<qTable->size()<<endl;
    }
    void reduceMap();
    void resetTable(){this->qTable->clear();}
    void setStochasticMovement(double m){this->_stochasticMovement=m;}
    void setHashFuction(std::function<u_int64_t (const State*)> fun){
        HashFuction=std::move(fun);
    }
    void policyData();
    void resetQtable();
    bool apply_action(State *s,const string &id,Point &action,int max_speed);
    void set_tran(vector<Policy*>* l){this->lTran=l;}
    void MyPolicy(Policy *my){this->my_policy=my;}
    double discountFactor=0.987;
    keyItem last_entry;
    Point get_argmx_action(State *s);
    int get_state_argmax(const State *s_state);
    static void arg_max(arr &arr,vector<int> &vec);
    ~RTDP_util();
    RTDP_util(string &mHome):home(mHome){}
    RTDP_util(int grid_size,vector<pair<int,int>>& max_speed_and_budget,string &mHome);
    void add_entry_map_state(keyItem basicString, const State *s);

    void set_value_matrix(keyItem entryState, Point &action ,double val){
        if(std::abs(val-this->qTable->operator[](entryState).operator[](action.hashMeAction(Point::actionMax)))<epslion)
            return;
        this->update_counter++;
        this->qTable->operator[](entryState).operator[](action.hashMeAction(Point::actionMax))=val;
    }
    vector<double>* get_probabilty(const State *s);
    void update_final_State(State *s, double val);
    cell rec_h(State *s, int index,cell acc_probablity);

    cell get_max_valueQ(const State *s)
    {
        auto& ar = get_Q_entry_values(s,getStateKeyValue(s));
        return *std::max_element(ar.begin(),ar.end());
    }
};


#endif //TRACK_RACING_RTDP_UTIL_HPP
