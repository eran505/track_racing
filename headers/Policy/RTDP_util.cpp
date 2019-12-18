//
// Created by ise on 15.12.2019.
//


#include "RTDP_util.hpp"

RTDP_util::RTDP_util(int grid_size,const list<pair<int,int>>& max_speed_and_budget) {
    this->set_up_action_map(this->D);
    this->set_up_Q(grid_size,max_speed_and_budget);
    this->mapState= new map<string,int>();
}

void RTDP_util::set_up_action_map(int D) {
    int ctr_action=0;
    if (D == 2)
    {
        this->mapAction=new map<int,int>();
        this->int_to_action= new map<int,Point*>();
        for (int i = -1; i < 2 ; ++i) {
            for (int j = -1; j < 2; ++j) {
                auto p_point = new Point(i,j);
                this->mapAction->insert({p_point->hash2D() ,ctr_action});
                this->int_to_action->insert({ctr_action,p_point});
                ctr_action++;
            }
        }

        this->size_mapAction=this->mapAction->size();
    }
}

void RTDP_util::set_up_Q(int grid_size, const list<pair<int,int>>& max_speed_and_budget) {
    int size_action = this->mapAction->size();
    int size_player=max_speed_and_budget.size();
    double state_number_overall = 1;
    for(auto &item : max_speed_and_budget){
        auto max_speed = item.first;
        state_number_overall *= pow(max_speed*2+1,this->D)*(grid_size+1)*item.second;
    }

    this->size_Q=int(state_number_overall);
    this->qTable = new double*[int(state_number_overall)]; // dynamic array (size 10) of pointers to int
    for (int i = 0; i < int(state_number_overall); ++i)
        this->qTable[i] = new double[size_action];
}


int RTDP_util::get_state_index_by_string(State *s_state) {
    string s = s_state->to_string_state();
    auto it = this->mapState->find(s);
    if (it != this->mapState->end()){
        return it->second;
    }else{
        return this->add_entry_map_state(s,s_state);
    }
    return 0;
}
void RTDP_util::heuristic(State *s,int entry_index)
{
    for (const auto &item_action : *this->int_to_action)
    {
        this->qTable[entry_index][item_action.first]=10.5;
    }
}

int RTDP_util::add_entry_map_state(string &basicString,State *s) {
    // compute heuristic
    this->heuristic(s,ctr_state);
    // add to state_map
    this->mapState->insert({basicString,ctr_state});
    return this->ctr_state++;

}

RTDP_util::~RTDP_util() {
    //Free each sub-array
    for(int i = 0; i < this->size_Q ; ++i) {
        delete[] this->qTable[i];
    }
    //Free the array of pointers
    delete[] this->qTable;
    for(auto it = this->int_to_action->begin(); it != this->int_to_action->end(); ++it) {

        delete(it->second);
    }
    delete(mapState);
    delete(mapAction);
    delete (int_to_action);
}

vector<int> arg_max(const double arr[],int size ){
    double max = *std::max_element(arr, arr+size);
    vector<int> l;
    for (int i = 0; i < size; ++i) {
        if (arr[i]==max)
            l.push_back(i);
    }
    return l;
}

int RTDP_util::get_state_argmax(State *s) {
    int argMax;
    int entry_state = this->get_state_index_by_string(s);
    auto row = this->qTable[entry_state];
    vector<int> argMax_list = arg_max(row,this->size_mapAction);
    int size = argMax_list.size();
    if (size>1)
    {
        argMax = argMax_list[ctr_random%size];
        ctr_random = ++ctr_random%this->size_mapAction;
    } else
        argMax = argMax_list[0];
    this->last_entry = entry_state;
    return argMax;
}

vector<float>* RTDP_util::get_probabilty(State *s) {
    int argMax;
    int entry_state = this->get_state_index_by_string(s);
    auto row = this->qTable[entry_state];
    vector<int> argMax_list = arg_max(row,this->size_mapAction);
    auto* l = new vector<float>();
    int prob = argMax_list.size();
    for (auto item: argMax_list){
        l->push_back(item);
        l->push_back(float(prob)/1);
    }
    return l;
}


double RTDP_util::get_value_state_max(State *s_state) {
    int entry_state = this->get_state_index_by_string(s_state);
    auto arr = this->qTable[entry_state];
    double max = *std::max_element(arr, arr + size_mapAction);;
    return max;
}

Point RTDP_util::get_argmx_action(State *s) {
    int index_action = this->get_state_argmax(s);
    auto pos = this->int_to_action->find(index_action);
    if (pos == this->int_to_action->end())
        throw std::invalid_argument( "function::get_argmx_action Error" );
    return *pos->second;
}

