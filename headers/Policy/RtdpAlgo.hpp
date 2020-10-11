//
// Created by ise on 15.12.2019.
//

#ifndef TRACK_RACING_RTDPALGO_HPP
#define TRACK_RACING_RTDPALGO_HPP
#include "Update_RTDP/Vanilla.hpp"
#include "RTDP_util.hpp"
#include "Policy/Policy.hpp"
#include "Update_RTDP/Evaluator.hpp"
#include <deque>
#include "Update_RTDP/Reward.hpp"
#include "Update_RTDP/ActionExpnder.hpp"
#include "MultiAction/EvaluatorActionizer.hpp"
#include "MultiAction/StackAction.hpp"
typedef shared_ptr<unordered_map<string,string>> dictionary;
typedef unordered_map<u_int64_t ,double> rewardMap ;

class RtdpAlgo : public Policy{
protected:
    std::unique_ptr<ActionExpnder> expnder = nullptr ;
    std::unique_ptr<EvaluatorActionzer> evaluator= nullptr;
    Actionzer::StackActionzer<Actionzer::tuple_stack> stack_backup;
    Rewards R= Rewards::getRewards();
    std::unique_ptr<rewardMap>  rewardDict = std::make_unique<rewardMap>();
    u_int64_t ctrInFun=0;
    Point zero_action=Point(0);
    int ctr_stack=0;
    u_int32_t zeroIndexAction = Point(0).hashMeAction(Point::actionMax);
    double _stochasticMovement=1;
    std::unique_ptr<Point>  ZeroAction = std::make_unique<Point>(0,0,0);
    RTDP_util *RTDP_util_object;
    std::shared_ptr<vector<pair<State,pair<u_int64_t,int>>>>  stackStateActionIdx;
    std::function <std::tuple<double,bool>(State *s)> evaluationState;
    double bellman_update(State *s,Point &action);
    double UpdateCalc(const vector <pair<State*,double>>& state_tran_q);
    void update(State *s,Point &action,u_int64_t entryMatrix);
    void inset_to_stack(State *s,Point &action,u_int64_t state_entry);
    void empty_stack_update();
    void minimization()override{this->RTDP_util_object->reduceMap();}
    void learnRest()override;
    //stackStateActionIdxdouble expected_reward(State *s, Point &action);
    //double expected_reward_rec(State *s,int index_policy,deque<Point> &my_stack);
public:
    void set_discounted_factor(double gama){evaluator->set_discount_factor(gama);}
    void returnAllQ(){this->evaluator->returnAll(this->RTDP_util_object);}
    void get_first_Q(){this->evaluator->set_first_Q(this->RTDP_util_object);}
    void genrateInfoPrint(){this->RTDP_util_object->printInfoGen();}
    [[nodiscard]] u_int64_t getUpdateCtr()const{return  this->RTDP_util_object->get_update_ctr();}
    void resetAlgo(){this->RTDP_util_object->resetTable();}
    void setStochasticMovement(double m)
    {
        _stochasticMovement=m;
        this->RTDP_util_object->setStochasticMovement(m);
        this->expnder->set_stochasticMovement(m);
    }
    [[nodiscard]] double getStochasticMovement() const{ return _stochasticMovement;}
    [[nodiscard]] double getRewardColl() const{ return R.CollReward;}
    [[nodiscard]] double getGoalReward() const{ return R.GoalReward;}
    [[nodiscard]] double getWallReward() const{ return R.WallReward;}
    ~RtdpAlgo() override
    {
        //cout<<"del RTDP"<<endl;
        delete(this->RTDP_util_object);
    }
    bool isInPolicy(const State *s) const override {return this->RTDP_util_object->isInQ(s);}
    RTDP_util* getUtilRTDP(){return RTDP_util_object;}
    RtdpAlgo(int maxSpeedAgent, int grid_size,State::agentEnum agentID,string &home);
    Point get_action(State *s) override;
    const vector<double >* TransitionAction(const State *s) const override ;
    void reset_policy() override;
    void policy_data() const override;

    [[nodiscard]] static Point get_lastPos() ;
    bool stoMove();
    void set_expder(int m){this->expnder->set_seq_action(m);}

    template<typename Parm>
    void init_expder(Parm &p){

        expnder=std::make_unique<ActionExpnder>(_stochasticMovement,tran,this);
        evaluator = std::make_unique<EvaluatorActionzer>(p,RTDP_util_object);
        evaluator->set_stack(stackStateActionIdx);
        this->RTDP_util_object->isEmptyQ();

    }
    void init_tran()
    {
        expnder=std::make_unique<ActionExpnder>(_stochasticMovement,tran,this);
    }
    const auto get_evaluator()const{ return evaluator.get();}
    const auto get_expnder()const{ return expnder.get();}

    void insetRewardMap(u_int64_t hashKey, double reward){
        auto ok = this->rewardDict->insert({hashKey,reward}).second;
        if(!ok)
            throw std::invalid_argument( "duplication key" );
    }
    void set_mode_agent(int mode_number);
    void update_final_state(State *s) override {
        auto [val,b]=this->evaluationState(s);
        this->RTDP_util_object->update_final_State(s,val);}

    double getArgMaxValueState(const State *s){ return this->RTDP_util_object->get_max_valueQ(s);}

    [[nodiscard]] double getReward(const Point &refPoint) const;



    tuple<double, bool> EvalState5(State *s);

    void inset_to_stack_abs(State *s, Point &action, u_int64_t state_entry);

    auto get_evalouater(){return this->evaluator.get();}

    double bellman_updateV2(State *s, Point &action);

    void do_SEQ(State *s, const Point &a);
};


#endif //TRACK_RACING_RTDPALGO_HPP
