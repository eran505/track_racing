//
// Created by ERANHER on 28.7.2020.
//

#ifndef TRACK_RACING_EVALUATORACTIONIZER_HPP
#define TRACK_RACING_EVALUATORACTIONIZER_HPP

#include <functional>
#include <utility>
#include "State.hpp"
#include "Policy/RTDP_util.hpp"
#include "MultiAction/Scheduler.hpp"
class EvaluatorActionzer{
    RTDP_util *ptrRTDP;
    string attacker;
    string defender;
    Rewards R = Rewards::getRewards();
    double discount_factor=0.987;
    double constant_cost=0;
    std::function <std::tuple<double,bool>(const State *s)> evaluationState;
    std::shared_ptr<vector<pair<State,pair<u_int64_t,int>>>> stack_roll_back = nullptr;
    Scheduler _scheduler;
public:
    EvaluatorActionzer(string defender_name,string attacker_name,RTDP_util *ptr= nullptr):
    ptrRTDP(ptr),
    attacker(std::move(attacker_name)),
    defender(std::move(defender_name)),
    _scheduler(attacker,defender,2)
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
        auto old_idx=_scheduler.get_idx();
        std::for_each(state_tran_q.begin(),state_tran_q.end(),[&](auto &item){
            res+=evalute_state(item.first,item.second);
        });
        return_back_starting_state(old_idx);
        return res;
    }
    bool change_scope_(State *s)
    {
        int delta = _scheduler.change_action_abstraction(s,this->ptrRTDP);
        //assert(s->get_budget(defender)+delta<2);
        s->set_budget(defender,s->get_budget(defender)+delta);
        if(delta==0)
            return false;
        return true;
    }
    void reset(RTDP_util *ptr)
    {
        _scheduler.reset(ptr);
    }
private:

    double evalute_state(const State *s,double transition_probability)
    {
        //cout<<s->to_string_state()<<endl;
        change_scope_const(s);
        double res=0;
        auto [val,isEndState]= this->evaluationState(s);
        if(!isEndState)
            val = this->ptrRTDP->get_max_valueQ(s);
        res+=val*transition_probability*this->discount_factor;

        return res;
    }
    int change_scope_const(const State *s)
    {
        int delta = _scheduler.change_action_abstraction(s,this->ptrRTDP);
        return delta ;
    }
    void return_back_starting_state(int idx){
        _scheduler.change_static(idx,this->ptrRTDP);
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
        if(!(s->get_position_ref(defender)>=s->get_position_ref(attacker)))
            return {R.GoalReward,true};
        return {0,false};
    }


    [[nodiscard]] inline Point get_lastPos() const{
        return this->stack_roll_back->back().first.get_position_ref(defender);
    }



};


#endif //TRACK_RACING_EVALUATORACTIONIZER_HPP
