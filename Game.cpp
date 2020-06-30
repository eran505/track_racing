//
// Created by ise on 19.11.2019.
//

#include "headers/Game.hpp"

Game::Game(MdpPlaner *planer_m) {

    this->in_game_adversaries = new list<Agent*>();
    this->in_game_guards = new list<Agent*>();
    this->out_game = new list<Agent*>();
    this->planer = planer_m;
    this->ctr_game=100;
    this->uper_limt=300;
    this->counterBuffer=0;
    this->buffer = new vector<string>(this->MAX_BUFFER);
    this->guardEval= new vector<vector<int>>();
    this->info=new vector<vector<int>>();
}

Game::~Game()
{
    delete(this->out_game);
    delete(this->in_game_guards);
    delete(this->in_game_adversaries);
    delete(this->planer);
    delete (this->buffer);
    delete(this->info);
    delete(this->guardEval);
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
        player->rest();
        if (player->get_team() == Section::adversary)
            this->in_game_adversaries->push_front(player);
        else
            this->in_game_guards->push_front(player);

        iter = this->out_game->erase(iter);
    }
}



void Game::evalPolicy() {

    this->planer->setPolicyModeAgent(true);
    int old_ctr_wall = this->ctr_wall;
    int old_ctr_coll=this->ctr_coll;
    int old_ctr_at_gal = this->ctr_at_gal;
    int open_ctr = this->ctr_at_open;
    for (int i = 0; i <this->numEval; ++i) {
        this->loop_game();
        this->reset_game();
    }
    //cout<<"Training Mode"<<endl;
    int evl_wall = this->ctr_wall-old_ctr_wall;
    int evl_coll = this->ctr_coll-old_ctr_coll;
    int evl_goal = this->ctr_at_gal-old_ctr_at_gal;
    int evl_open = this->ctr_at_open-open_ctr;
    this->print_eval(evl_coll,evl_goal,evl_wall);
    ctr_coll=old_ctr_coll;
    ctr_at_gal=old_ctr_at_gal;
    ctr_wall=old_ctr_wall;
    this->planer->setPolicyModeAgent(false);
    this->guardEval->push_back({ctr_round,evl_wall,evl_coll,evl_goal,evl_open});

}

void Game::startGame(int numIter)
{
    if(numIter==0) return;
    //int size = numIter%10000 == 0 ? numIter/10000 :numIter/10000+1;
    this->guardEval->reserve(numIter/modEval);
    //auto info = new vector<vector<int>>();
    this->init_game();
    this->fill_agents();
    //this->reset_game();

    for (int i = 1; i <= numIter; ++i) {
        //cout<<"game: "<<i<<endl;
        this->loop_game();
        this->reset_game();
        ctr_round++;
        if (ctr_round%modEval==0){
            evalPolicy();
            //this->print_stats();
            vector<int> tmp(5);
            tmp[0]=ctr_round;
            tmp[1]=this->ctr_wall;
            tmp[2]=this->ctr_coll;
            tmp[3] = this->ctr_at_gal;
            tmp[4] = this->ctr_at_open;
            info->push_back(tmp);
        }
        if (isConverage())
            break;
    }
    cout<<"Collision:\t"<<ctr_coll<<endl;
    if (ctr_round%1000>0)
    {
        vector<int> tmp(5);
        tmp[0]=ctr_round;
        tmp[1]=this->ctr_wall;
        tmp[2]=this->ctr_coll;
        tmp[3] = this->ctr_at_gal;
        tmp[4] = this->ctr_at_open;
        info->push_back(tmp);
    }
}

void Game::loop_game() {
    //cout<<"-----New Game------"<<endl;
    //cout<<this->planer->get_cur_state()->to_string_state()<<endl;
    int i=0;
    for (i = 0; i < this->ctr_game; ++i) {
        //cout<<"round: "<<i<<endl;
        //print the state of the game
        //cout<<this->planer->get_copy_state().to_string_state()<<endl;
//        cout<<this->planer->get_cur_state()->getHashValue()<<endl;
        for (auto i : *(this->in_game_guards)){
            //cout<<i->get_name()<<endl;
            i->doAction(planer->get_cur_state());
            addToBuffer(i->get_id()+"@"+this->planer->get_cur_state()->get_position_ref(i->get_id()).to_str());
        }
        //cout<<this->planer->get_cur_state()->to_string_state()<<endl;
        for (auto i : *(this->in_game_adversaries)){
            //cout<<i->get_name()<<endl;
            i->doAction(planer->get_cur_state());
            auto pos = this->planer->get_cur_state()->get_position_ref(i->get_id()).to_str();
            addToBuffer(i->get_id()+"@"+pos);


        }

        //cout<<this->planer->get_cur_state()->to_string_state()<<endl;


        // check constrain end game (Wall/Budget/Coll/Goal)
        this->constraint_checking_end_game();

        if (this->is_end_game()){
            //cout << "end"<<endl;
            addToBuffer("END");
            this->del_all_player();
            break;
        }
    }
    if (i==ctr_game)
    this->del_all_player();
}

bool Game::validate_player(Agent *player){
    if (player->get_is_wall())
        return true;

    if (player->getPolicy()->out_budget)
        return true;

    return false;
}

void Game::constraint_checking_end_game(){
    list<Agent*> to_del_ad;
    list<Agent*> to_del_gu;
    for (auto i : *(this->in_game_adversaries)){
        auto pos = &(planer->get_cur_state()->get_position_ref(i->get_name_id())); // call the copy con
        if (validate_player(i)){
            //cout<<"Del\t"<<i->get_id()<<"\tbudget/wall"<<endl;
            //push to list of del player
            to_del_ad.push_front(i);
            continue;

        }
        //At the Gaol
        if (this->planer->get_Grid()->is_at_goal(pos))
        {
            //remove this player from the game
            //cout<<"Del\t"<<i->get_id()<<"\tAt Goal"<<endl;
            // push to list of del player
            //cout<<this->planer->get_cur_state()->get_position_ref(i->get_id()).to_str()<<endl;
            auto theGoal = this->planer->get_cur_state()->get_position_ref(i->get_id()).to_str();
            auto isTargetGoal = this->planer->get_Grid()->isGoalReward(*pos);
            (isTargetGoal) ? this->ctr_at_gal++:this->ctr_at_open++;
            //cout<<theGoal<<endl;
            to_del_ad.push_front(i);
        }
    }
    // clean all adversaries agents
    this->del_list_func(to_del_ad,false);

    for (auto i : *(this->in_game_guards)) {
        if (validate_player(i)){
            //remove player
            //cout<<"Del\t"<<i->get_id()<<"\tbudget/wall"<<endl;
            to_del_gu.push_front(i);
            ctr_wall++;
            continue;
        }
        auto pos = &(planer->get_cur_state()->get_position_ref(i->get_name_id()));
        //coll
        list<Agent*> l;
        for(auto adversary:*(this->in_game_adversaries)){
            auto pos_bad = &planer->get_cur_state()->get_position_ref(adversary->get_name_id());
            if( pos_bad->is_equal(pos)){
                l.push_front(adversary);
            }
        }
        if (l.size()>0){
            for (auto player: l){
                //remove  player
                //cout<<"Del\t"<<player->get_id()<<" - Coll"<<endl;
                to_del_ad.push_front(player);
            }
            // remove guard
           // cout<<"Del\t"<<i->get_id()<<" - Coll"<<endl;
            to_del_gu.push_front(i);
            ctr_coll++;
        }
    }
    // remove all guards agents
    this->del_list_func(to_del_gu,true);

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
    this->planer->reset_state();
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

bool Game::isConverage() const {
    int lim=4;
    long acc=0;
    bool isEnd = false;
    if (info->size() > lim)
    {
        if(info->operator[](info->size()-lim-1)[1]==info->operator[](info->size()-1)[1])
            if(info->operator[](info->size()-lim-1)[3]==info->operator[](info->size()-1)[3])
                isEnd=true;
    }
    return isEnd;
}



