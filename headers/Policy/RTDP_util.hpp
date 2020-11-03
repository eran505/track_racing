//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDP_UTIL_HPP
#define TRACK_RACING_RTDP_UTIL_HPP
#define OUTDATA
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
#include <cassert>
#include "Update_RTDP/Reward.hpp"
#define DD
//#define LAST_STATE_DEBUG // uncomment the (line 326 Simulator.hpp)
//#define H_ZERO
typedef u_int64_t keyItem;
typedef double cell;



class RTDP_util{
protected:
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
    Rewards R = Rewards::getRewards();
    short ctr_debug=0;
    unordered_map<keyItem,std::array<int,14>> debugDict;
    const string home;
    std::function<u_int64_t (const State*)> HashFuction;
    Policy* my_policy= nullptr;
    unsigned int ctr_state=0;
    size_t ctr_random=0;
    double epslion=0;
    int size_Q;
    int size_mapAction;
    unordered_map<int,Point*>* hashActionMap;
    void heuristic(const State *s,keyItem entry_index);
    int to_closet_path_H_calc(const Point& agnet_pos,int jumps);
    int to_closet_path_H(const State &s);
    keyItem getStateKeyValue(const State *s) const
    {
        return HashFuction(s);
    }

    arr& get_Q_entry_values(const State *s,keyItem key)
    {
#ifdef LAST_STATE_DEBUG
        if(start_inset)
            if(auto poss = this->state_policy_dict.find(key);poss==state_policy_dict.end())
                this->state_policy_dict.try_emplace(key,s->to_mini_string(),1);
            else poss->second.second++;

#endif

        auto pos = qTable->find(key);
        if(pos==qTable->end())
        {
            add_entry_map_state(key,s);
            return qTable->operator[](key);
        }
        //if(this->debugDict.find(key)->second[12]!=s->get_budget(this->my_policy->cashID))
//        bool flag=false;
//        auto a1 = s->to_mini_string();
//        auto a2 = this->debugDict.find(key)->second;
//        for(int i=0;i<14;++i)
//        {
//            if(a1[i]!=a2[i])
//                flag=true;
//        }
//        if(flag)
//        {
//            cout<<"key:"<<key<<endl;
//            auto a = this->debugDict.find(key)->second;
//            cout<<"BUGBUG"<<endl;
//            cout<<s->to_string_state()<<" H:";
//            cout<<s->getHashValue()<<endl;
//            cout<<State::make_state_from_array(a).to_string_state()<<" H:";
//            cout<<State::make_state_from_array(a).getHashValue()<<endl;
//
//            cout<<"--------"<<endl;
//
//           // assert(false);
//        }
        return pos->second;
    }
    double applyNonAction(const State *s);

public:
    u_int32_t inconsistent=0;
    uint steo_takken=0;
    bool start_inset=false;
    #ifdef LAST_STATE_DEBUG
    std::unordered_map<u_int64_t,pair<std::array<int,14>,u_int64_t>> state_policy_dict;
    #endif
    void reset_takken_stpe_ctr(){steo_takken=0;}
    unordered_map<keyItem,std::array<int,14>> get_dict_map(){return this->debugDict;}
    void inset_move_dict_map(unordered_map<keyItem,std::array<int,14>> d){cout<<"in"<<endl;this->debugDict=std::move(d);}
    std::vector<std::vector<Point>> l_p_H;
    void isEmptyQ()
    {

        if(qTable== nullptr)
            qTable=std::make_unique<unordered_map<keyItem ,arr>>();
    }
    //auto get_string_DEBUG(u_int64_t k){return debugDict.at(k);}
    double discountFactor=1;//0.987
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
    void setStochasticMovement(double m){this->_stochasticMovement=m;}
    void setHashFuction(std::function<u_int64_t (const State*)> fun){
        HashFuction=std::move(fun);
    }
    void policyData();
    void resetQtable();
    static bool apply_action(State *s,State::agentEnum id,Point &action,int max_speed);
    void set_tran(vector<Policy*>* l){this->lTran=l;}
    void MyPolicy(Policy *my){this->my_policy=my;}

    keyItem last_entry;
    Point get_argmx_action(State *s);
    int get_state_argmax(const State *s_state);
    static void arg_max(const arr &arr,vector<int> &vec);
    ~RTDP_util();
    RTDP_util(string &mHome):home(mHome){}
    RTDP_util(int grid_size,string &mHome);
    void add_entry_map_state(keyItem basicString, const State *s);

    void set_value_matrix(keyItem entryState, const Point &action ,cell val){

        auto& vec = this->qTable->at(entryState);
        auto old = vec[action.hashMeAction(Point::actionMax)];
        if(int(old)>0 and int(val)>0)
        if(int(old)-int(val)<0)
        {
            cout<<old<<":->"<<val;
            cout<<"  S_ID:"<<entryState<<" actionID:"<<action.hashMeAction(Point::actionMax)<<endl;
            //assert(false);
            inconsistent++;
        }
        //if(std::abs(dif)==epslion) return;
        //this->update_counter++;
        //cout<<"s:"<<entryState<<" ,a:"<<action.to_str()<<" ]="<<val<<endl;
        this->qTable->operator[](entryState).operator[](action.hashMeAction(Point::actionMax))=val;
        //this->qTable->operator[](entryState).operator[](action.hashMeAction(Point::actionMax))+=dif;
    }
    vector<double>* get_probabilty(const State *s);
    void update_final_State(State *s, cell val);

    cell get_max_valueQ(const State *s)
    {
        auto& ar = get_Q_entry_values(s,getStateKeyValue(s));
        //if(s->to_string_state()=="0A_(15, 19, 1)_(2, 2, -1)_0|0D_(9, 9, 0)_(-1, -1, 0)_1|")
        //    cout<<ar<<endl;
        return *std::max_element(ar.begin(),ar.end());
    }


    void set_q_table(std::unique_ptr<unordered_map<keyItem ,arr>> &&table)
    {
        qTable=std::move(table);
    }
    std::unique_ptr<unordered_map<keyItem ,arr>> && get_q_table()
    { return std::move(qTable);}

    bool apply_action_SEQ(State *s, State::agentEnum id, Point &action, int max_speed);

    void plusplus();
    int distance_H(const State &s)const;
};


#endif //TRACK_RACING_RTDP_UTIL_HPP
