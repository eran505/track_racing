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

    for(auto &item : this->pos_dict){
        string id_name = item.first;
        auto my_pos = &item.second;
        auto my_speed = &this->speed_dict[id_name];
        int my_budget = this->budget_dict[id_name];
        str+=id_name+sep+my_pos->to_str()+sep+my_speed->to_str()+sep+to_string(my_budget);
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
    //cout<<"COPY Constractor STATE"<<endl;
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
    this->g_grid=other.g_grid;
}

list<string> State::is_collusion(string &id_player) {
    //// return all the name of the opposite team that share the same location with the given player
    list<string> list_name;
    char my_team = id_player[0];
    for(auto item : this->pos_dict){
        char team_id = item.first.operator[](0);
        if (team_id == my_team)
            continue;
        if (this->pos_dict[id_player].is_equal(&item.second))
            list_name.push_back(item.first);
    }
    return list_name;
}

set<string> State::is_collusion() {
    //// return all player that collide
    set<string> values;
    for(auto item : this->pos_dict){
        char team_id = item.first.operator[](0);
        for (auto item2 : this->pos_dict)
        {
            char team_id_other = item2.first.operator[](0);
            if (team_id_other == team_id)
                continue;
            if (item.second.is_equal(&item2.second))
            {
                values.insert(item.first);
                values.insert(item2.first);
                break;
            }
        }
    }
    return values;
}



bool State::move_by_change_speed(const string& name_id, const Point& speed_m){
    //this->speed_dict[name_id]=speed_m;
    this->pos_dict[name_id]+= speed_m;
    return this->g_grid->is_wall(&(this->pos_dict[name_id]));

}

bool State::isGoal() {
    for(const auto &item : this->pos_dict)
    {
        if (item.first[1]==Section::adversary)
        {
            if(this->g_grid->is_at_goal(&item.second))
                return true;
        }
    }
    return false;
}


