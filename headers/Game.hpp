//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_GAME_HPP
#define RACING_CAR_GAME_HPP

#include "MdpPlaner.hpp"

class Game {

    list<Agent*> *in_game_adversaries;
    list<Agent*> *in_game_guards;
    list<Agent*> *out_game;
    MdpPlaner *planer;

public:
    Game(MdpPlaner *planer_m);

    unsigned int ctr_game;
    unsigned int uper_limt;
    ~Game();
    void clean_in_player();
    void clean_out_player();
    void init_game();
    void fill_agents();
    void print_list_in_game();
    void loop_game();
    void constraint_checking_end_game();
    void del_palyer(Agent *agent,bool is_guard);
    bool is_end_game();
    void reset_game();
    void del_all_player();
    void del_list_func(list<Agent*> l,bool guard);
    static bool validate_player(Agent *player);
};


#endif //RACING_CAR_GAME_HPP
