//
// Created by ise on 19.11.2019.
//

#include "headers/Game.hpp"

Game::Game(MdpPlaner *planer_m) {

    this->in_game = new list<Agent*>();
    this->out_game = new list<Agent*>();
    this->planer = planer_m;
    this->ctr_game=10;
    this->uper_limt=100;


}

Game::~Game() {
    delete(this->out_game);
    delete(this->in_game);
    delete(this->planer);

}


void Game::clean_in_player() {
    while (!this->in_game->empty())
    {
        this->in_game->pop_back();
    }
}

void Game::clean_out_player() {
    while (!this->out_game->empty())
    {
        this->out_game->pop_back();
    }


}

void Game::init_game() {
    this->fill_agents();
    this->clean_out_player();
}

void Game::fill_agents() {
    this->clean_in_player();
    this->planer->get_all_players(this->in_game);

}

void Game::print_list_in_game() {
    for (auto i : *(this->in_game)){
        cout<<"in"<<endl;
        cout<<i->to_str()<<endl;
    }

}

void Game::loop_game() {

    for (int i = 0; i < this->ctr_game; ++i) {
        //print the state of the game
        cout<<this->planer->get_copy_state().to_string_state();
        for (auto i : *(this->in_game)){
            i->do_action(planer->get_cur_state());

        }
        cout<<"print game"<<endl;
    }
}


