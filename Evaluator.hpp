//
// Created by eranhe on 26/07/2020.
//

#ifndef TRACK_RACING_EVALUATOR_HPP
#define TRACK_RACING_EVALUATOR_HPP

#include <functional>
#include <utility>
#include "State.hpp"
#include "Policy/RTDP_util.hpp"
#include "Reward.hpp"
class Evaluator{
    RTDP_util *ptrRTDP;
    State::agentEnum attacker=State::agentEnum::A;
    State::agentEnum defender=State::agentEnum::D;
    Rewards R = Rewards::getRewards();
    double discount_factor=0.987;
    double constant_cost=0;
    std::function <std::tuple<double,bool>(const State *s)> evaluationState;
    std::shared_ptr<vector<pair<State,pair<u_int64_t,int>>>> stack_roll_back = nullptr;
public:
    explicit Evaluator(RTDP_util *ptr=nullptr):ptrRTDP(ptr)
    {
        evaluationState = [&](const State *s){return EvalState2(s);};
    }
    void set_RTDPptr(RTDP_util* ptr){this->ptrRTDP=ptr;}
    void set_discount_factor(double m){discount_factor=m;}
    void set_constant_cost(double m){constant_cost=m;}
    void set_stack(std::shared_ptr<vector<pair<State,pair<u_int64_t,int>>>>& ptr)
    {
        stack_roll_back=ptr;
    }
    [[nodiscard]] double get_discount_factor()const{ return discount_factor;}
    [[nodiscard]] double get_constant_cost()const{ return constant_cost;}
    void set_eval(int index_func)
    {
        if (index_func==1) {
            evaluationState=[&](const State *s){return EvalState2(s);};
        }
    }


    double calculate(const std::vector<pair<State *, double>>& state_tran_q)
    {
        double res=0;
        std::for_each(state_tran_q.begin(),state_tran_q.end(),[&](auto &item){
            res+=evalute_state(item.first,item.second);
        });
        return res;
    }

private:

    double evalute_state(const State *s,double transition_probability)
    {
        double res=0;
        auto [val,isEndState]= this->evaluationState(s);
        if(!isEndState)
            val = this->ptrRTDP->get_max_valueQ(s);
        res+=val*transition_probability*this->discount_factor;
        return res;
    }



    tuple<double,bool> EvalState2(const State *s)
    {
        if (s->g_grid->is_wall(s->get_position_ref(defender)))
        {
            return {R.WallReward,true};
        }
        if (s->is_collusion(defender,attacker))
        {
            return {R.CollReward,true};
        }
        if (auto x = s->isGoal(attacker);x>=0)
            return {R.GoalReward*x,true};
        return {0,false};
    }
    [[nodiscard]] inline Point get_lastPos() const{
        return this->stack_roll_back->back().first.get_position_ref(defender);
    }

    tuple<double,bool> EvalState3(State *s) {

        if (s->g_grid->is_wall(s->get_position_ref(defender))){
            return {R.WallReward,true};
        }
        auto res = s->is_collusion(defender,attacker);
        if (res){
            if(get_lastPos()==s->get_position_ref(defender))
                return {R.CollReward,true};
        }
        if (s->isGoal(attacker)>=0) {
            return {R.GoalReward, true};
        }
        if(s->isEndState(attacker)){
            return {0,true};
        }
        return {0,false};
    }
};


#endif //TRACK_RACING_EVALUATOR_HPP
