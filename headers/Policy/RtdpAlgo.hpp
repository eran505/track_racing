//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDPALGO_HPP
#define TRACK_RACING_RTDPALGO_HPP

#include "RTDP_util.hpp"
#include "Policy/Policy.hpp"
#include <deque>
typedef shared_ptr<unordered_map<string,string>> dictionary;
typedef unordered_map<u_int64_t ,double> rewardMap ;
class RtdpAlgo : public Policy{

    std::unique_ptr<rewardMap>  rewardDict = std::make_unique<rewardMap>();

    double CollReward = 1;
    double GoalReward = -1;
    double WallReward = -10;
    int ctr_stack=0;
    u_int32_t zeroIndexAction = Point(0).hashMeAction(Point::actionMax);
    double _stochasticMovement=1;
    std::unique_ptr<Point>  ZeroAction = std::make_unique<Point>(0,0,0);
    RTDP_util *RTDP_util_object;
    vector<pair<State*,pair<int,int>>> stackStateActionIdx;
    double bellman_update(State *s,Point &action);
    double UpdateCalc(const vector <pair<State*,double>>& state_tran_q);
    void update(State *s,Point &action,int entryMatrix);
    void inset_to_stack(State *s,Point &action,int state_entry);
    void empty_stack_update();

    //stackStateActionIdxdouble expected_reward(State *s, Point &action);
    //double expected_reward_rec(State *s,int index_policy,deque<Point> &my_stack);
public:
    void genrateInfoPrint(){this->RTDP_util_object->printInfoGen();}
    void resetAlgo(){this->RTDP_util_object->resetTable();}
    void setStochasticMovement(double m)
    {
        _stochasticMovement=m;
        this->RTDP_util_object->setStochasticMovement(m);
    }
    [[nodiscard]] double getStochasticMovement() const{ return _stochasticMovement;}
    [[nodiscard]] double getRewardColl() const{ return CollReward;}
    [[nodiscard]] double getGoalReward() const{ return GoalReward;}
    [[nodiscard]] double getWallReward() const{ return WallReward;}
    ~RtdpAlgo() override
    {
        cout<<"del RTDP"<<endl;
        delete(this->RTDP_util_object);
    }
    RTDP_util* getUtilRTDP(){return RTDP_util_object;}
    RtdpAlgo(int maxSpeedAgent, int grid_size, vector<pair<int,int>> &max_speed_and_budget,const string &agentID,string &home,dictionary &ptrDict);
    Point get_action(State *s) override;
    const vector<double >* TransitionAction(State *s) override ;
    void reset_policy() override;
    void policy_data() const override;
    std::tuple<double,bool> EvalState2(State *s);
    std::tuple<double,bool> EvalState(State *s);
    bool stoMove();

    void insetRewardMap(u_int64_t hashKey, double reward){
        auto ok = this->rewardDict->insert({hashKey,reward}).second;
        if(!ok)
            throw std::invalid_argument( "duplication key" );
    }

    void update_final_state(State *s) override {
        auto [val,b]=EvalState2(s);
        this->RTDP_util_object->update_final_State(s,val);}

    double getArgMaxValueState(const State *s){ return this->RTDP_util_object->get_value_state_max(s);}

    [[nodiscard]] double getReward(const Point &refPoint) const;
};


#endif //TRACK_RACING_RTDPALGO_HPP
