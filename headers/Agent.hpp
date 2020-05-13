//
// Created by ise on 17.11.2019.
//

#ifndef RACING_CAR_AGENT_HPP
#define RACING_CAR_AGENT_HPP
#include "util_game.hpp"
#include "State.hpp"
#include "Policy/Policy.hpp"
#include <random>

//#include ""
//class State;

typedef std::vector<std::pair<float,Point>> weightedPointVector;
class Agent{

protected:
    weightedPointVector *my_pos;
    Policy* my_Policy;
    Point* my_speed;
    int my_budget;
    bool is_wall;
    const char my_team;
    const string my_id;
    string name;
    bool eval;

public:
    bool get_is_wall() const{ return is_wall;}
    void rest(){is_wall= false; this->my_Policy->reset_policy();}
    const string& get_name_id(){ return my_id;}
    Agent(weightedPointVector* pos,Point* speed,string m_id , char m_team,int b_budget);
    Agent(weightedPointVector* pos,Point* speed, char m_team,int b_budget);
    const string& get_id(){ return my_id; }
    char get_team() const { return my_team; }
    string get_name();
    const Policy* getPolicy(){ return my_Policy;};
    void doAction(State *s);
    void setPolicy(Policy* pPtr){pPtr->set_id(this->my_id); this->my_Policy=pPtr;}
    ~Agent();
    void evalPolicy();
    void trainPolicy();
    void set_speed(Point *cur_speed) {
        if (this->my_speed != nullptr)
            delete(this->my_speed);
        this->my_speed=cur_speed;
    }

    int get_budget() const{ return this->my_budget;}

    const Point* get_speed(){

        return this->my_speed;
    }

    Point get_pos(float seed){
        float acc = 0;
        u_int16_t ctr=0;
        for (auto const &item:*my_pos){
            acc += std::get<0>(item);
            if (acc >= seed)
                return std::get<1>(item);
            ctr++;
        }
        return std::get<1>(my_pos->operator[](ctr-1));
    }
    Point get_speed_v1(){ return *my_speed;}


    void to_print();


    static int ctr_object;
};


#endif //RACING_CAR_AGENT_HPP
