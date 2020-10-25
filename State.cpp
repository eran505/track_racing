//
// Created by ise on 18.11.2019.
//

#include "headers/State.hpp"

void State::add_player_state(agentEnum name_id, const Point &m_pos, const Point *m_speed, int budget_b) {
    this->dataPoint[name_id*2]=m_pos;
    this->dataPoint[name_id*2+1]=*m_speed;
    this->budgets[name_id]=budget_b;
}

string State::to_string_state() const {
    string sep="_";
    string str;
    for(int j =0;j<this->budgets.size();++j){
        string id_name = j==0?"A":"D";
        auto my_pos = this->dataPoint[j*2];
        auto my_speed =  this->dataPoint[j*2+1];
        int my_budget = this->budgets[j];
        str.append(id_name);
        str.append(sep);
        str.append(my_pos.to_str());
        str.append(sep);
        str.append(my_speed.to_str());
        str.append(sep);
        str.append(std::to_string(my_budget));
        str+="|";
        //str+=id_name+sep+my_pos->to_str()+sep+my_speed->to_str()+sep+to_string(my_budget);
    }
    return str;

}
std::array<int,14> State::to_mini_string() const
{
    std::array<int,14> arr{};
    int ctr=0;
    for(int j =0;j<this->budgets.size();++j){
        auto position = this->dataPoint[j*2];
        auto speed = this->dataPoint[j*2+1];
        for(int i=0;i<position.capacity;++i) arr[ctr+i]= position.array[i];
        ctr+=position.capacity;
        for(int i=0;i<speed.capacity;++i) arr[ctr+i]=speed.array[i];
        ctr+=speed.capacity;
    }
    arr[12]=this->budgets[0];
    arr[13]=this->budgets[1];
    return arr;
}


//:dataPoint(other.dataPoint),budgets(other.budgets),g_grid(other.g_grid),takeOff(other.takeOff) { }


bool State::is_collusion(agentEnum id_player,agentEnum op_player)const
{
    if(this->dataPoint[id_player*2]==this->dataPoint[op_player*2])
        return true;
    else
        return false;
}
bool State::is_collusion_radius(agentEnum id_player, agentEnum op_player, const Point &window)
{
    Point dif = (this->get_position_ref(id_player)-this->get_position_ref(op_player)).AbsPoint();
    if(dif<window)
        return true;
    return false;
}


//set<string> State::is_collusion() {
//    //// return all player that collide
//    set<string> values;
//    for(auto item : this->pos_dict){
//        char team_id = item.first.operator[](item.first.size()-1);
//        for (auto item2 : this->pos_dict)
//        {
//            char team_id_other = item2.first.operator[](item.first.size()-1);
//            if (team_id_other == team_id)
//                continue;
//            if (item.second.is_equal(&item2.second))
//            {
//                values.insert(item.first);
//                values.insert(item2.first);
//                break;
//            }
//        }
//    }
//    return values;
//}





double State::isGoal(agentEnum idStr)const {
    const auto& pos = dataPoint[idStr*2];
    return this->g_grid->get_goal_reward(pos);
}

bool State::isEndState(agentEnum idStr) const{
    const auto& pos = dataPoint[idStr*2];
    return this->g_grid->is_at_goal(pos);
}

bool State::applyAction( agentEnum id, const Point &action, int max_speed) {

    this->dataPoint[id*2+1]+=action;
    this->dataPoint[id*2+1].change_speed_max(max_speed);
    this->dataPoint[id*2]+=this->dataPoint[id*2+1];
    auto outBound = this->g_grid->is_wall(this->dataPoint[id*2]);
    return outBound;
}
bool State::applyAction( agentEnum id,Point &action, int max_speed,int jumps) {

    for (int k=0;k<jumps and k < 2 ;++k)
    {
        this->dataPoint[id*2+1]+=action;
        this->dataPoint[id*2+1].change_speed_max(max_speed);
        this->dataPoint[id*2]+=this->dataPoint[id*2+1];

    }
    if(jumps-2>0)
    {
        this->dataPoint[id*2+1]*=(jumps-2);
        this->dataPoint[id*2]+=this->dataPoint[id*2+1];
        this->dataPoint[id*2+1].change_speed_max(max_speed);
    }
    auto outBound = this->g_grid->is_wall(this->dataPoint[id*2]);
    return outBound;
}





void State::assignment(State &other)
{
    for(int i=0;i<this->budgets.size();i++)
    {
        auto enum_id = static_cast<agentEnum>(i);
        this->set_position(enum_id,other.get_position_ref(enum_id));
        this->set_speed(enum_id,other.get_speed_ref(enum_id));
        this->set_budget(enum_id,other.get_budget(enum_id));
    }
}



u_int64_t  State::getHashValuePosOnly() const{
    vector<int> vec;
    for (short x = A; x != LAST; ++x)
    {
        for (int i = 0; i < Point::D_point::D; ++i)
            vec.push_back(this->dataPoint[x*2][i]);
        if(x!=A)
        {
            for (int i = 0; i < Point::D_point::D; ++i)
                vec.push_back(this->dataPoint[x*2+1][i]);
        }
    }
    u_int64_t  seed = vec.size();
    for(auto& i : vec) {
        seed ^=  (i * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

u_int64_t State::getHashValue2()const {
    u_int64_t  seed = 12;
    for (short x = A; x != LAST; ++x)
    {
        for (int i = 0; i < Point::D_point::D; ++i)
            seed ^=  (this->dataPoint[x*2][i] * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
        for (int i = 0; i < Point::D_point::D; ++i)
            seed ^=  (this->dataPoint[x*2+1][i] * 2654435761) + 2654435769 + (seed << 6) + (seed >> 2);
    }
    return seed;
}

u_int64_t State::getHashValue()const {
    u_int64_t  seed = 0;
    size_t i=0;
    while(true)
    {
        seed ^=  this->dataPoint[i].array[0] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^=  this->dataPoint[i].array[1] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        seed ^=  this->dataPoint[i].array[2] + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        if(i++==3) break;
    }
    return seed;
}


void State::add_player_state(agentEnum name_id, const Point& m_pos, const Point& m_speed, short budget_b) {
    this->set_budget(name_id,budget_b);
    this->set_speed(name_id,m_speed);
    this->set_position(name_id,m_pos);
}



void State::getAllPos(vector<Point> &vec)const
{        for (short x = A; x != LAST; ++x) vec.push_back(this->dataPoint[x]); }

void State::assignment(const State *other, State::agentEnum idname) {
    this->dataPoint[idname*2+1]=other->dataPoint[idname*2+1];
    this->dataPoint[idname*2]=other->dataPoint[idname*2];
    this->budgets[idname]=other->budgets[idname];
}











