//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDPALGO_HPP
#define TRACK_RACING_RTDPALGO_HPP

#include "RTDP_util.hpp"
#include "Policy/Policy.hpp"
#include <deque>

class RtdpAlgo : public Policy{
    double CollReward = 10;
    double GoalReward = -10;
    double WallReward = -11;
    int ctr_stack=0;
    string tmp;
    RTDP_util *RTDP_util_object;
    vector<pair<State*,pair<int,int>>> stackStateActionIdx;
    double bellman_update(State *s,Point &action);
    double UpdateCalc(const vector <pair<State*,float>>& state_tran_q);
    void update(State *s,Point &action,int entryMatrix);
    void inset_to_stack(State *s,Point &action,int state_entry);
    void empty_stack_update();
    //stackStateActionIdxdouble expected_reward(State *s, Point &action);
    //double expected_reward_rec(State *s,int index_policy,deque<Point> &my_stack);
public:
    double getRewardColl(){ return CollReward;}
    double getGoalReward(){ return GoalReward;}
    double getWallReward(){ return WallReward;}
    ~RtdpAlgo() override
    {
        cout<<"del RTDP"<<endl;
        delete(this->RTDP_util_object);
    }
    RtdpAlgo(string namePolicy, int maxSpeedAgent, int grid_size, vector<pair<int,int>> &max_speed_and_budget,string agentID,string &home);
    Point get_action(State *s) override;
    const vector<float >* TransitionAction(State *s) override ;
    void reset_policy() override;
    void policy_data() const override;
    double EvalState(State *s);
};


#endif //TRACK_RACING_RTDPALGO_HPP
