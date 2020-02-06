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
    int D=2;
    string id_agent;
    unordered_map<int,Point*>* hashActionMap;
    vector<Policy*> tran;
    Policy(string name_policy,int max_speed_agent,string agentID)
    :max_speed(max_speed_agent){
        this->name=std::move(name_policy);
        this->is_wall=false;
        this->out_budget= false;
        this->hashActionMap = Point::getDictAction();
        this->id_agent=move(agentID);
    }
    const string* get_id_name(){ return &id_agent;}
    const string& GetId(){ return id_agent;}

    virtual ~Policy(){
        cout<<"~Policy"<<endl;
        for (auto &item : *this->hashActionMap)
            delete(item.second);
        delete(hashActionMap);
    }

    void set_id(string id_m){this->id_agent=std::move(id_m);}
    virtual Point get_action(State *s)=0;
    virtual void reset_policy() {};
    virtual void policy_data()const=0;
    virtual const vector<float >* TransitionAction(State *s)=0;
    void add_tran(Policy *ptr_tran)
    {
        this->tran.push_back(ptr_tran);
    }
    void applyActionToState(State *my_state, Point *action ){
        // change the budget according the budget function
        this->budgetFunc(my_state, action);

        // append the prvoious speed to the new action

        auto tmp = &my_state->get_speed(id_agent);  // invoking the empty_con
        //*action += *tmp;
        *tmp += *action;
        // check whether the new speed is over the MAX_SPEED
        tmp->change_speed_max(this->max_speed);

        // change pos according to the currnet speed
        this->is_wall = my_state->move_by_change_speed(id_agent,*tmp); // invoke empty con POINT

    }
    void budgetFunc(State *state_now, Point *action){
        //some calc
        auto new_budget = (state_now->get_budget(id_agent));
        state_now->set_budget(id_agent,new_budget);
    }

};



#endif //RACING_CAR_POLICY_HPP
