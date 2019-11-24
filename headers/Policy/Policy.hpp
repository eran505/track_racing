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
    string name;
    string id_agent;
    Policy(string id_name, string name_policy){
        this->id_agent=std::move(id_name);
        this->name=std::move(name_policy);
    }
    virtual Point get_action(State s)=0;
    virtual void reset_policy();
    virtual void policy_data();


};
#endif //RACING_CAR_POLICY_HPP
