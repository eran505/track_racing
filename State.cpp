//
// Created by ise on 18.11.2019.
//

#include "headers/State.hpp"

void State::add_player_state(const string& name_id, Point m_pos, const Point *m_speed, int budget_b) {
    this->pos_dict.emplace(name_id,m_pos);

    this->speed_dict.emplace(name_id,*m_speed);

    this->budget_dict[name_id]=budget_b;
}

string State::to_string_state() const {
    string sep="_";
    string str;
    for(const auto &item : this->pos_dict){
        string id_name = item.first;
        auto my_pos = &item.second;
        auto pos_speed = this->speed_dict.find(id_name);
        if (pos_speed==this->speed_dict.end())
            throw;
        auto my_speed = &pos_speed->second;
        int my_budget = this->budget_dict.at(id_name);
        str.append(id_name);
        str.append(sep);
        str.append(my_pos->to_str());
        str.append(sep);
        str.append(my_speed->to_str());
        str.append(sep);
        str.append(std::to_string(my_budget));
        str+="|";
        //str+=id_name+sep+my_pos->to_str()+sep+my_speed->to_str()+sep+to_string(my_budget);
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
   // cout<<"COPY Constractor STATE"<<endl;

    for (auto item : other.pos_dict){
        string name_id = item.first;
        auto pos_i = other.pos_dict.at(name_id);
        auto speed_i = other.speed_dict.at(name_id);
        auto budget_i = other.budget_dict.at(name_id);
        // insert to the mew object
        this->speed_dict.insert({name_id,speed_i});
        this->pos_dict.insert({name_id,pos_i});
        this->budget_dict.insert({name_id,budget_i});
        takeOff= other.takeOff;
    }
    this->g_grid=other.g_grid;
}
bool State::is_collusion(string &id_player,string &op_player)
{
    if(this->pos_dict[id_player]==this->pos_dict[op_player])
        return true;
    else
        return false;
}
list<string> State::is_collusion(string &id_player) {
    //// return all the name of the opposite team that share the same location with the given player
    list<string> list_name;
    char my_team = id_player[id_player.size()-1];
    for(auto item : this->pos_dict){
        char team_id = item.first.operator[](id_player.size()-1);
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
        char team_id = item.first.operator[](item.first.size()-1);
        for (auto item2 : this->pos_dict)
        {
            char team_id_other = item2.first.operator[](item.first.size()-1);
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

bool State::isGoal(string &idStr) {
    const auto& pos = this->get_position(idStr);
    return this->g_grid->is_goal_reward(pos);
}

bool State::isEndState(std::string &idStr) {
    const auto& pos = this->get_position(idStr);
    return this->g_grid->is_at_goal(pos);
}

bool State::applyAction(const string &id, Point &action, int max_speed) {
    auto pos = this->speed_dict.find(id);
    if (pos==this->speed_dict.end())
        throw;
    pos->second+=action;
    pos->second.change_speed_max(max_speed);
    auto pos_on_grid = this->pos_dict.find(id);
    pos_on_grid->second+=pos->second;
    auto outBound = this->g_grid->is_wall(&(pos_on_grid->second));
    return outBound;
}

void State::assignment(State &other)
{
    for(const auto &item: other.budget_dict)
        this->assignment(other,item.first);
}

void State::assignment(State &other, const string &id) {
    this->pos_dict[id]=other.get_position(id);
    this->speed_dict[id]=other.get_speed(id);
    this->budget_dict[id]=other.get_budget(id);
}

void State::getAllPosOpponent(vector<Point> &results,char team) {
    for(auto &pos: this->pos_dict)
    {
        if (pos.first[1]==team)
            continue;
        results.push_back(pos.second);
        auto itemSpeed = this->speed_dict.find(pos.first);
        if (itemSpeed == this->speed_dict.end())
            throw;
        results.push_back(itemSpeed->second);
    }
}

u_int32_t State::getHashValue(){
    vector<int> vec;
    for(auto const &item:this->pos_dict)
    {
        for(int i = 0; i < Point::D_point::D; ++i)
            vec.push_back(item.second.array[i]);
        for(int i = 0; i < Point::D_point::D; ++i)
            vec.push_back(speed_dict[item.first].array[i]);

    }
    u_int32_t  seed = vec.size();
    for(auto& i : vec) {
        seed ^=  (i * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

void State::add_player_state(const string &name_id, const Point& m_pos, const Point& m_speed, int budget_b) {
    this->pos_dict.emplace(name_id,m_pos);

    this->speed_dict.emplace(name_id,m_speed);

    this->budget_dict[name_id]=budget_b;
}




