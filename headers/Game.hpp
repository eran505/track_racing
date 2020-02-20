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
    int ctr_coll=0;
    int ctr_at_gal=0;
    int ctr_wall=0;
    int ctr_round=0;
    int numEval=500;
    int modEval = 10000;


public:
    explicit Game(MdpPlaner *planer_m);
    void print_stats(){
        cout<<"ctr_round= "<<ctr_round;
        cout<<"\tctr_coll= "<<ctr_coll;
        cout<<"\tctr_at_goal= "<<ctr_at_gal;
        cout<<"\tctr_wall= "<<ctr_wall<<endl;
    }
    unsigned int ctr_game;
    unsigned int uper_limt;
    ~Game();
    void clean_in_player();
    void evalPolicy();
    void clean_out_player();
    void init_game();
    void fill_agents();
    void print_list_in_game();
    void loop_game();
    bool isConverage();
    void constraint_checking_end_game();
    void del_palyer(Agent *agent,bool is_guard);
    bool is_end_game();
    void reset_game();
    void del_all_player();
    void del_list_func(list<Agent*> l,bool guard);
    static bool validate_player(Agent *player);
    vector<string>* buffer;
    vector<vector<int>>* guardEval;
    vector<vector<int>>* info;
    void startGame(int numIter);
};


#endif //RACING_CAR_GAME_HPP
