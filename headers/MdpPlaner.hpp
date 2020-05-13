//
// Created by ise on 17.11.2019.
//

#ifndef RACING_CAR_MDPPLANER_HPP
#define RACING_CAR_MDPPLANER_HPP

#include "Grid.hpp"
#include "Agent.hpp"
#include "util_game.hpp"
#include <map>
#include "State.hpp"
class MdpPlaner{
public:

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    bool add_player(Agent *ptr_agent);
    Agent* get_player(string str_id_name);
    State* get_cur_state(){ return cur_state;};
    State get_copy_state(){ return *cur_state;};

    ~MdpPlaner();
    void set_state();
    MdpPlaner(int seed):
    goal_reward(10),collusion_reward(-10), wall_reward(-0.1),distribution(0.0,1.0),
    generator(seed){
        this->agents_map= new std::map<char, map<string,Agent*>*>();
    }
    MdpPlaner(double goal_reward_m, double coll_reward_m, double wall_reward_m){
        this->collusion_reward=coll_reward_m;
        this->wall_reward=wall_reward_m;
        this->goal_reward=goal_reward_m;
        this->agents_map= new std::map<char, map<string,Agent*>*>();
    }
    void set_grid(Grid *g);
    Grid * get_Grid(){ return grid;};
    void get_all_players(list<Agent *> *traget_list);
    void reset_state(){
        delete(this->cur_state);
        this->set_state();
    }
    void setPolicyModeAgent(bool mode);
protected:
    double goal_reward;
    double collusion_reward;
    double wall_reward;
    State *cur_state;
    Grid *grid;
    std::map<char, map<string,Agent*>*> *agents_map;
};

#endif //RACING_CAR_MDPPLANER_HPP
