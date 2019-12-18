//
// Created by ise on 15.12.2019.
//

#include "RtdpAlgo.hpp"
#include <queue>
RtdpAlgo::RtdpAlgo(string namePolicy, int maxSpeedAgent, int grid_size, const list<pair<int,int>> &max_speed_and_budget)
        : Policy(namePolicy, maxSpeedAgent) {
   this->RTDP_util_object = new RTDP_util(grid_size,max_speed_and_budget);

}

void RtdpAlgo::policy_data() {
    Policy::policy_data();
}

void RtdpAlgo::reset_policy() {
    Policy::reset_policy();
}

vector<float >* RtdpAlgo::TransitionAction(State *s)
{
    return this->RTDP_util_object->get_probabilty(s);
}

Point RtdpAlgo::get_action(State *s) {
    //return the argmax action in the given state row
    auto action = this->RTDP_util_object->get_argmx_action(s);
    int entry=this->RTDP_util_object->last_entry;
    //update state action

    //TODO: inset the state action tuple to the stack to update at the end of the episode
    this->update(s,action,entry);
    return action;
}

double RtdpAlgo::bellman_update(State *s, Point &action) {
    auto stateCur = new State(*s);

    this->applyActionToState(stateCur, &action);
    // is wall
    if (this->is_wall == true)
    {
        //do something
    }
    // generate all possible next state with the probabilities
    vector <pair<State*,float>> state_tran_q;
    state_tran_q.emplace_back(stateCur,1);
    for (Policy *item_policy: tran)
    {
        vector <pair<State*,float>> state_tran_q_tmp;
        for (const auto &value: state_tran_q)
        {
            float probability = value.second;
            // Warning MUST del the Tran vec (options_actions)!!!
            auto options_actions = item_policy->TransitionAction(value.first);
            for(std::vector<float>::size_type i = 0; i != options_actions->size(); i++) {
                // copy state
                auto *new_state = new State(*value.first);
                // take the action
                auto pos = MapindexAction.find(options_actions->operator[](i));
                if ( pos == MapindexAction.end()) {
                    throw std::invalid_argument("Action index is invalid");;
                } else {
                    Point *actionI = pos->second;
                    item_policy->applyActionToState(new_state,actionI);
                    state_tran_q_tmp.emplace_back(new_state,options_actions->operator[](++i)*probability);
                }
            }
            delete(options_actions);
            // del the state that were alloc on the heap
            delete(value.first);
        }
        state_tran_q=state_tran_q_tmp;
    }

    return this->UpdateCalc(state_tran_q);
}

double RtdpAlgo::EvalState(State *s) {
    auto res = s->is_collusion(this->id_agent);
    if (s->isGoal())
        return this->GoalReward;
    if (this->is_wall)
        return WallReward;
    if (res.size()>1)
        return this->CollReward;
    return 0;
}

double RtdpAlgo::UpdateCalc(const vector<pair<State *, float>>& state_tran_q) {
    double res=0;
    for (auto &item:state_tran_q)
    {
        double val = this->EvalState(item.first);
        // check max value in the Q table
        if (val == 0)
            val = this->RTDP_util_object->get_value_state_max(item.first);
        res+=val*item.second;
        delete(item.first);
    }
    return res;
}

void RtdpAlgo::update(State *s, Point &action,int entryMatrix) {
    auto val = this->bellman_update(s,action);
    this->RTDP_util_object->set_value_matrix(entryMatrix,action,val);
}



