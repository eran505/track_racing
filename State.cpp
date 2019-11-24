//
// Created by ise on 18.11.2019.
//

#include "headers/State.hpp"

void State::add_player_state(string name_id,Point m_pos,Point m_speed,int budget_b) {

    this->pos_dict[name_id]=m_pos;

    this->speed_dict[name_id]=m_speed;

    this->budget_dict[name_id]=budget_b;
}


string State::to_string_state() {
    char sep='_';
    string str="";

    for(auto item : this->pos_dict){
        string id_name = item.first;
        auto my_pos = item.second;
        auto my_speed = this->speed_dict[id_name];
        int my_budget = this->budget_dict[id_name];
        str+=id_name+sep+my_pos.to_str()+sep+my_speed.to_str()+sep+to_string(my_budget);
        str+="|";
    }
    return str;

}

State::~State() {
//    for(auto item : this->pos_dict)
//        delete(item.second);
//    for(auto item : this->speed_dict)
//        delete(item.second);
//

}

State::State(const State &other) {
    for (auto item : other.pos_dict){
        string name_id = item.first;
        auto pos_i = other.pos_dict.at(name_id);
        auto speed_i = other.speed_dict.at(name_id);
        auto budget_i = other.budget_dict.at(name_id);
        // insert to the mew object
        this->speed_dict.insert({name_id,speed_i});
        this->pos_dict.insert({name_id,pos_i});
        this->budget_dict.insert({name_id,budget_i});
    }
}


