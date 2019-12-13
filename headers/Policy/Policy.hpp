//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_POLICY_HPP
#define RACING_CAR_POLICY_HPP

#include <utility>

#include "../State.hpp"
#include "../util_game.hpp"
class Policy{
public:
    int max_speed;
    string name;
    bool out_budget;
    bool is_wall;
    string id_agent;
    list<Policy*> tran;
    Policy(string name_policy,int max_speed_agent)
    :max_speed(max_speed_agent){
        this->name=std::move(name_policy);
        this->is_wall=false;
        this->out_budget= false;
    }
    const string* get_id_name(){ return &id_agent;}
    virtual ~Policy() = default;
    void set_id(string id_m){this->id_agent=std::move(id_m);}
    virtual Point get_action(State *s)=0;
    virtual void reset_policy() {cout<<"reset_policy base"<<endl;};
    virtual void policy_data(){cout<<"policy_data base"<<endl;}

    void add_tran(Policy *ptr_tran){
        this->tran.push_front(ptr_tran);
    }
    void apply_action_to_state(State *my_state, Point *action ){
        // change the budget according the budget function
        this->budget_func(my_state,action);

        // append the prvoious speed to the new action

        auto tmp = &my_state->get_speed(id_agent);  // invoking the empty_con
        *action += *tmp;
        // check whether the new speed is over the MAX_SPEED
        action->change_speed_max(this->max_speed);

        // change pos according to the currnet speed
        this->is_wall = my_state->move_by_change_speed(id_agent,*action); // invoke empty con POINT

    }
    void budget_func(State *state_now,Point *action){
        //some calc
        auto new_budget = (state_now->get_budget(id_agent));
        state_now->set_budget(id_agent,new_budget);
    }
};

#endif //RACING_CAR_POLICY_HPP
