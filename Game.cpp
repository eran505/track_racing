//
// Created by ise on 19.11.2019.
//

#include "headers/Game.hpp"

Game::Game(MdpPlaner *planer_m) {

    this->in_game_adversaries = new list<Agent*>();
    this->in_game_guards = new list<Agent*>();
    this->out_game = new list<Agent*>();
    this->planer = planer_m;
    this->ctr_game=10;
    this->uper_limt=300;


}

Game::~Game() {
    delete(this->out_game);
    delete(this->in_game_guards);
    delete(this->in_game_adversaries);
    delete(this->planer);


}


void Game::clean_in_player() {
    while (!this->in_game_adversaries->empty()) {
        auto item = this->in_game_adversaries->front();
        this->out_game->push_back(item);
        this->in_game_adversaries->pop_front();
    }
    while (!this->in_game_guards->empty()){
        auto item = this->in_game_guards->front();
        this->out_game->push_back(item);
        this->in_game_guards->pop_front();
    }

}

void Game::clean_out_player() {
    while (!this->out_game->empty())
    {
        this->out_game->pop_back();
    }


}

void Game::init_game() {
    this->planer->get_all_players(this->out_game);
    //this->fill_agents();
    //this->clean_out_player();
}


void Game::fill_agents() {
    auto iter = this->out_game->begin();
    auto end = this->out_game->end();

    while (iter != end)
    {
        auto player = *iter;

        if (player->get_team() == Section::adversary)
            this->in_game_adversaries->push_front(player);
        else
            this->in_game_guards->push_front(player);

        iter = this->out_game->erase(iter);
    }
}

void Game::print_list_in_game() {
    for (auto i : *(this->in_game_guards))
        cout<<i->to_str()<<endl;

    for (auto i : *(this->in_game_adversaries))
        cout<<i->to_str()<<endl;
}



void Game::loop_game() {
    //cout<<this->planer->get_cur_state()->to_string_state()<<endl;
    for (int i = 0; i < this->ctr_game; ++i) {
        cout<<"round: "<<i<<endl;
        //print the state of the game
        //cout<<this->planer->get_copy_state().to_string_state();
        for (auto i : *(this->in_game_adversaries)){
            //cout<<i->get_name()<<endl;
            i->do_action(planer->get_cur_state());
        }
        for (auto i : *(this->in_game_guards)){
            //cout<<i->get_name()<<endl;
            i->do_action(planer->get_cur_state());
        }

        //cout<<this->planer->get_cur_state()->to_string_state()<<endl;


        // check constrain end game (Wall/Budget/Coll/Goal)
        this->constraint_checking_end_game();

        if (this->is_end_game()){
            this->del_all_player();
            break;
        }
    }
}

bool Game::validate_player(Agent *player){
    if (player->get_Policy()->is_wall)
        return true;

    if (player->get_Policy()->out_budget)
        return true;
}

void Game::constraint_checking_end_game(){
    list<Agent*> to_del_ad;
    list<Agent*> to_del_gu;
    for (auto i : *(this->in_game_adversaries)){
        auto pos = &(planer->get_cur_state()->get_position(*i->get_name_id())); // call the copy con
        if (validate_player(i)){
            cout<<"Del\t"<<i->to_str()<<"budget/wall"<<endl;
            //push to list of del player
            to_del_ad.push_front(i);
            continue;

        }
        //At the Gaol
        if (this->planer->get_Grid()->is_at_goal(pos))
        {
            //remove this player from the game
            cout<<"Del\t"<<i->to_str()<<"At Goal"<<endl;
            // push to list of del player
            to_del_ad.push_front(i);

        }
    }
    // clean all adversaries agents
    this->del_list_func(to_del_ad,false);

    for (auto i : *(this->in_game_guards)) {
        if (validate_player(i)){
            //remove player
            cout<<"Del\t"<<i->to_str()<<"budget/wall"<<endl;
            to_del_gu.push_front(i);
            continue;
        }
        auto pos = &(planer->get_cur_state()->get_position(*i->get_name_id()));
        //coll
        list<Agent*> l;
        for(auto adversary:*(this->in_game_adversaries)){
            auto pos_bad = &planer->get_cur_state()->get_position(*adversary->get_name_id());
            if( pos_bad->is_equal(pos)){
                l.push_front(adversary);
            }
        }
        if (l.size()>0){
            for (auto player: l){
                //remove  player
                cout<<"Del\t"<<player->to_str()<<" - Coll"<<endl;
                to_del_ad.push_front(player);
            }
            // remove guard
            cout<<"Del\t"<<i->to_str()<<" - Coll"<<endl;
            to_del_gu.push_front(i);
        }
    }
    // remove all guards agents
    this->del_list_func(to_del_ad,true);

}

void Game::del_palyer(Agent *agent ,bool is_guard) {
    if (is_guard){
        this->in_game_guards->remove(agent);
    } else{
        this->in_game_adversaries->remove(agent);
    }
    this->out_game->push_front(agent);

}

bool Game::is_end_game() {
    return this->in_game_adversaries->size() == 0 or this->in_game_guards->size() == 0;
}

void Game::reset_game() {
    this->fill_agents();
    this->clean_out_player();
}

void Game::del_all_player() {
    for (auto i : *(this->in_game_adversaries)){
        this->out_game->push_front(i);
    }
    this->in_game_adversaries->clear();
    for (auto i : *(this->in_game_guards)){
        this->out_game->push_front(i);
    }
    this->in_game_guards->clear();

}

void Game::del_list_func(list<Agent *> l,bool guard) {
    for (auto i : l)
        this->del_palyer(i, guard);
}



