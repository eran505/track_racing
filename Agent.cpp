//
// Created by ise on 17.11.2019.
//

#include "headers/Agent.hpp"

#include <utility>



int Agent::ctr_object = 0;
Agent::Agent(weightedPositionVector Startpos, string m_id, char m_team, int b_budget)
: my_Policy(nullptr),is_wall(false), my_team(m_team),my_id(std::move(m_id)),eval(false) {
    this->my_budget=b_budget;
    this->initialPosition = std::move(Startpos);

}

Agent::Agent( weightedPositionVector Startpos, char m_team, int b_budget)
        : my_Policy(nullptr),is_wall(false),my_team(m_team),my_id(std::to_string(ctr_object)+m_team) ,eval(false) {
    this->initialPosition = std::move(Startpos);
    this->my_budget=b_budget;
    ctr_object++;
}




string Agent::get_name() {
    string name;
    name = this->my_team+this->my_id;
    return name;
}

Agent::~Agent() {
    delete (this->my_Policy);

}



void Agent::evalPolicy(){
    this->eval= true;
    my_Policy->evalPolicy= true;
}
void Agent::trainPolicy(){
    this->eval= false;
    my_Policy->evalPolicy= false;
}



void Agent::doAction(State *s) {

    //do something with the state
    Point action_a = this->my_Policy->get_action(s);
//    cout<<"action_a"<<action_a.to_str()<<endl;
    this->my_Policy->applyActionToState(s, &action_a);
    is_wall=my_Policy->is_wall;
    // check for max speed

    //cout<<s->to_string_state()<<endl;
}


