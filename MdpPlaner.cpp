//
// Created by ise on 17.11.2019.
//

#include "headers/MdpPlaner.hpp"

bool MdpPlaner::add_player(Agent *ptr_agent) {
    char team = ptr_agent->get_team();
    string agent_ID = (ptr_agent->get_id());
    if ( this->agents_map->find(team) == this->agents_map->end()) {
        this->agents_map->operator[](team)= new map<string, Agent *> ();
    }
    map<string, Agent *>* team_dict;
    team_dict = (this->agents_map)->operator[](team);
    if(team_dict->find(ptr_agent->get_name())==team_dict->end()){
        team_dict->operator[](ptr_agent->get_name())=ptr_agent;
    }else{
        printf("the player %s is in the game!! ",(ptr_agent->get_name()).c_str());
    }
}

Agent *MdpPlaner::get_player(string str_id_name) {
    char team = str_id_name[0];
    auto it_dict =  this->agents_map->find(team);
    if (it_dict == this->agents_map->end())
        return nullptr;
    auto res_agent = it_dict->second->find(str_id_name);
    if (res_agent == it_dict->second->end() )
        return nullptr;
    return res_agent->second;
}

MdpPlaner::~MdpPlaner() {
    for(auto it = this->agents_map->begin(); it != this->agents_map->end(); ++it) {
        for (auto item : *(it->second)) { //item == {key,value}
            delete(item.second);
        }
        delete(it->second);
    }
    delete(this->agents_map);
    delete (this->grid);
    delete(this->cur_state);
}

void MdpPlaner::set_grid(Grid *g) {
    this->grid=g;
}

list<Agent *> *MdpPlaner::get_all_players(list<Agent*> *traget_list) {

    for (auto it = this->agents_map->begin(); it != this->agents_map->end(); ++it) {
        for (auto item : *(it->second)) { //item == {key,value}
            traget_list->push_front(item.second);
        }
    }
}

void MdpPlaner::set_state() {
    this->cur_state=new State();
    list<Agent*> all_agents;
    this->get_all_players(&all_agents);
    for (auto item : all_agents){
        cur_state->add_player_state(item->get_name(),item->get_pos_v1(),
                item->get_speed_v1(),item->get_budget());
    }
    this->cur_state->g_grid=this->grid;
    cout<<"----"<<endl;
    cout<<this->cur_state->to_string_state()<<endl;
    cout<<"----"<<endl;
}








