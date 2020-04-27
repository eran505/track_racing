//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_GAME_HPP
#define RACING_CAR_GAME_HPP

#include <utility>

#include "MdpPlaner.hpp"

class Game {

    list<Agent*> *in_game_adversaries;
    list<Agent*> *in_game_guards;
    list<Agent*> *out_game;
    MdpPlaner *planer;
    int ctr_coll=0;
    const int MAX_BUFFER=10000;
    int counterBuffer=0;
    int ctr_at_gal=0;
    int ctr_wall=0;
    int ctr_at_open=0;
    int ctr_round=0;
    int numEval=500;
    int modEval = 1000;


public:
    explicit Game(MdpPlaner *planer_m);
    void print_stats(){
        cout<<"ctr_round= "<<ctr_round;
        cout<<"\tctr_coll= "<<ctr_coll;
        cout<<"\tctr_at_goal= "<<ctr_at_gal;
        cout<<"\tctr_wall= "<<ctr_wall<<endl;
    }
    void print_eval(int m_ctr_coll,int m_ctr_at_goal,int m_ctr_wall)
    {
        cout<<"ctr_round= "<<this->ctr_round;
        cout<<"\tctr_coll= "<<m_ctr_coll;
        cout<<"\tctr_at_goal= "<<m_ctr_at_goal;
        cout<<"\tctr_wall= "<<m_ctr_wall<<endl;
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
    void addToBuffer(string strI)
    {
        if (this->counterBuffer==this->MAX_BUFFER-1)
            this->counterBuffer=0;
        this->buffer->operator[](this->counterBuffer)=std::move(strI);
        this->counterBuffer++;
    }
};


#endif //RACING_CAR_GAME_HPP
