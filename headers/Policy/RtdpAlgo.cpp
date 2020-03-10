//
// Created by ise on 15.12.2019.
//

#include "RtdpAlgo.hpp"
#include <queue>
#include <utility>
RtdpAlgo::RtdpAlgo(string namePolicy, int maxSpeedAgent, int grid_size, vector<pair<int,int>>& max_speed_and_budget,string agentID,string &home)
        : Policy(std::move(namePolicy), maxSpeedAgent,std::move(agentID),home) {
   this->RTDP_util_object = new RTDP_util(grid_size,max_speed_and_budget);
    this->RTDP_util_object->set_tran(&this->tran);
    this->RTDP_util_object->MyPolicy(this);


}



void RtdpAlgo::reset_policy() {
    this->empty_stack_update();
    Policy::reset_policy();
}

const vector<float >* RtdpAlgo::TransitionAction(State *s)
{
    return this->RTDP_util_object->get_probabilty(s);
}

Point RtdpAlgo::get_action(State *s)
{
    //return the argmax action in the given state row
    auto action = this->RTDP_util_object->get_argmx_action(s);
    //cout<<"action:="<<action.to_str()<<endl;




    if (this->evalPolicy)
    {
        if (!s->takeOff)
            if(action.hashMeAction(Point::actionMax)!=13 ){
                s->set_speed(this->id_agent,Point(0,0,max_speed));
                //this->inset_to_stack(s,action,entry);
                //action.array[2]=this->max_speed;
                s->takeOff=true;
            }
        return action;
    }




    int entry=this->RTDP_util_object->last_entry;
    //update state action
    // set the max speed in the Z coordinate at the when taking off
    //inset to stack for backup update
    this->inset_to_stack(s,action,entry);

    if (!s->takeOff)
        if(action.hashMeAction(Point::actionMax)!=13 ){
            s->set_speed(this->id_agent,Point(0,0,max_speed));
            //this->inset_to_stack(s,action,entry);
            //action.array[2]=this->max_speed;
            s->takeOff=true;
            this->tmp=s->to_string_state();
        }
    //TODO: inset the state action tuple to the stack to update at the end of the episode

    this->update(s,action,entry);



    return action;
}

//double RtdpAlgo::expected_reward(State *s, Point &action)
//{
//    deque<Point> stacAction;
//    int index_pos = 0;
//    stacAction.push_back(action);
//    this->RTDP_util_object->applyAction(s,this->id_agent,action,this->max_speed);
//
//}
//

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
                auto pos = this->hashActionMap->find(options_actions->operator[](i));
                if ( pos == this->hashActionMap->end()) {
                    throw std::invalid_argument("Action index is invalid");;
                } else {
                    Point *actionI = pos->second;
                    item_policy->applyActionToState(new_state,actionI);
                    state_tran_q_tmp.emplace_back(new_state,options_actions->operator[](++i)*probability);
                }
            }
            //delete(options_actions);
            //del the state that were alloc on the heap
            delete(value.first);
        }
        state_tran_q=state_tran_q_tmp;
    }

    return this->UpdateCalc(state_tran_q);
}

double RtdpAlgo::EvalState(State *s)
{

    if (s->isGoal())
        return this->GoalReward;
    if (this->is_wall)
        return WallReward;
    auto res = s->is_collusion(this->id_agent);
    if (res.size()>0)
        return this->CollReward;
    return 0;
}

double RtdpAlgo::UpdateCalc(const vector<pair<State *, float>>& state_tran_q) {
    double res=0;
    for (auto &item:state_tran_q)
    {

        double val = this->EvalState(item.first);
        //cout<<item.first->to_string_state()<<"= "<<val<<endl;
        // check max value in the Q table
        if (val == 0)
            val = this->RTDP_util_object->get_value_state_max(item.first);
        res+=val*item.second*this->RTDP_util_object->discountFactor;
        delete(item.first);
    }
    return res;
}

void RtdpAlgo::update(State *s, Point &action,int entryMatrix)
{
    auto val = this->bellman_update(s,action);
    this->RTDP_util_object->set_value_matrix(entryMatrix,action,val);
}

void RtdpAlgo::inset_to_stack(State *s,Point &action,int state_entry)
{
    stackStateActionIdx.push_back({new State(*s),{state_entry,action.hashMeAction(Point::actionMax)}});
    ctr_stack++;
}

void RtdpAlgo::empty_stack_update() {
    for (int i = ctr_stack-1 ; i > -1 ; --i) {
        // update the action state
        auto pos = this->hashActionMap->find(this->stackStateActionIdx[i].second.second);
        if (pos==this->hashActionMap->end())
            throw;
        this->update(this->stackStateActionIdx[i].first,*pos->second,
                this->stackStateActionIdx[i].second.first);
        delete(this->stackStateActionIdx[i].first);
    }
    ctr_stack=0;
    this->stackStateActionIdx.clear();
}

void RtdpAlgo::policy_data() const {
    this->RTDP_util_object->policyData();
}

//double RtdpAlgo::expected_reward_rec(State *s, int index_policy, deque<Point> &my_stack) {
//    return 0;
//}





