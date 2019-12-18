//
// Created by ise on 17.11.2019.
//

#include "headers/Agent.hpp"



int Agent::ctr_object = 0;
Agent::Agent( Point* pos, Point* speed, string m_id, char m_team, int b_budget)
: my_id(m_id), my_team(m_team),is_wall(false) {
    this->my_pos=pos;
    this->my_speed=speed;
    this->my_budget=b_budget;

}

string Agent::to_str() {
    string str_res;
    str_res = *this->get_name_id()+"_"+this->my_pos->to_str()+"_"+this->my_speed->to_str()+"_"+
            std::to_string(this->my_budget);
    return str_res;
}

string Agent::to_print() {
    cout<<this->to_str()<<endl;
}



string Agent::get_name() {
    std::__cxx11::string name;
    name = this->my_team+this->my_id;
    return name;
}

Agent::~Agent() {
    delete (this->my_Policy);
    delete(this->my_speed);
    delete(this->my_pos);
}

Agent::Agent( Point* pos, Point* speed, char m_team, int b_budget)
        : my_id(std::to_string(ctr_object)+m_team), my_team(m_team),is_wall(false) {
    this->my_pos=pos;
    this->my_speed=speed;
    this->my_budget=b_budget;
    ctr_object++;
}

void Agent::doAction(State *s) {
    //do something with the state
    Point action_a = this->my_Policy->get_action(s);
    //cout<<"action_a"<<action_a.to_str()<<endl;
    this->my_Policy->applyActionToState(s, &action_a);
    is_wall=my_Policy->is_wall;
    // check for max speed

    //cout<<s->to_string_state()<<endl;
}


