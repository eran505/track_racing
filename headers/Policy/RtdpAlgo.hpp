//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDPALGO_HPP
#define TRACK_RACING_RTDPALGO_HPP

#include "RTDP_util.hpp"
#include "Policy/Policy.hpp"

class RtdpAlgo : public Policy{
    double CollReward = 10;
    double GoalReward = -10;
    double WallReward = -1;
    RTDP_util *RTDP_util_object;
    double bellman_update(State *s,Point &action);
    double UpdateCalc(const vector <pair<State*,float>>& state_tran_q);
    void update(State *s,Point &action,int entryMatrix);
public:
    ~RtdpAlgo()
    {
        cout<<"del RTDP"<<endl;
        delete(this->RTDP_util_object);
    }
    RtdpAlgo(string namePolicy, int maxSpeedAgent, int grid_size, const list<pair<int,int>> &max_speed_and_budget);
    Point get_action(State *s) override;
    vector<float >* TransitionAction(State *s) override ;
    void reset_policy() override;
    void policy_data() override;
    double EvalState(State *s);
};


#endif //TRACK_RACING_RTDPALGO_HPP
