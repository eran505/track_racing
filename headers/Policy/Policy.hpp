//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_POLICY_HPP
#define RACING_CAR_POLICY_HPP

#include <utility>

#include "../State.hpp"
#include "../util_game.hpp"
typedef std::shared_ptr<unordered_map<string ,string>> dictionary;
class Policy{

public:
    int max_speed;
    string name;
    bool out_budget;
    bool is_wall;
    int D=2;
    string cashID;
    dictionary infoDict;
    bool evalPolicy;
    string home;
    string id_agent;
    unordered_map<int,Point*>* hashActionMap;
    vector<Policy*> tran;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;

    Policy(string name_policy,int max_speed_agent,string agentID,string &_home,dictionary info,int seed=3)
    :max_speed(max_speed_agent),evalPolicy(false),infoDict(std::move(info)),home(_home),generator(seed){
        this->name=std::move(name_policy);
        this->is_wall=false;
        this->out_budget= false;
        this->hashActionMap = Point::getDictAction();
        this->id_agent=move(agentID);
        distribution = std::uniform_real_distribution<double>(0.0,1.0);

    }
    const string& get_id_name(){ return id_agent;}
    const string& GetId(){ return id_agent;}
    double getRandom(){return distribution(generator);}



    virtual ~Policy(){
        cout<<"~Policy"<<endl;
        for (auto &item : *this->hashActionMap)
            delete(item.second);
        delete(hashActionMap);
    }

    void set_id(string id_m){this->id_agent=id_m;}
    virtual Point get_action(State *s)=0;
    virtual void reset_policy() {};
    virtual void policy_data()const=0;
    virtual const vector<float >* TransitionAction(State *s)=0;
    void add_tran(Policy *ptr_tran)
    {
        this->tran.push_back(ptr_tran);
        if((tran).size()==1)
            cashID=tran[0]->id_agent;
    }
    void applyActionToState(State *my_state, Point *action ){
        // change the budget according the budget function
        this->budgetFunc(my_state, action);

        // append the prvoious speed to the new action
        this->is_wall = my_state->applyAction(this->get_id_name(),*action,this->max_speed);


    }
    void budgetFunc(State *state_now, Point *action){
        //some calc
        auto new_budget = (state_now->get_budget(id_agent));
        state_now->set_budget(id_agent,new_budget);
    }

};




#endif //RACING_CAR_POLICY_HPP
