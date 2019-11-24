//
// Created by ise on 18.11.2019.
//

#ifndef RACING_CAR_STATE_HPP
#define RACING_CAR_STATE_HPP

#include "util_game.hpp"
#include "Grid.hpp"
class State{

public:
    map<string,Point> pos_dict;
    map<string,Point> speed_dict;
    map<string,int> budget_dict;
    Grid *g_grid;

    State(){};
    ~State();
    State(const State &other);
    void add_player_state(string name_id,Point m_pos,Point m_speed,int budget_bptr_agent);
    string to_string_state();

    //Setters and Getters
    void set_budget(string name_id,int budget_m){budget_dict[name_id]=budget_m;}
    int get_budget(string name_id){ return budget_dict[name_id];}
    void set_speed(string name_id,Point speed_m){speed_dict[name_id]=speed_m;}
    Point get_speed(string name_id){ return speed_dict[name_id];}
    void set_position(string name_id,Point pos_m){pos_dict[name_id]=pos_m;}
    Point get_position(string name_id){ return pos_dict[name_id];}


};


#endif //RACING_CAR_STATE_HPP
