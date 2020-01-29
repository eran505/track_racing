//
// Created by ise on 17.11.2019.
//

#ifndef RACING_CAR_AGENT_HPP
#define RACING_CAR_AGENT_HPP
#include "util_game.hpp"
#include "State.hpp"
#include "Policy/Policy.hpp"
//#include ""
//class State;

class Agent{

protected:
    static int ctr_object;
    Point* my_pos;
    Policy* my_Policy;
    Point* my_speed;
    int my_budget;
    bool is_wall;
    const char my_team;
    const string my_id;
    string name;


public:
    bool get_is_wall(){ return is_wall;}
    void rest(){is_wall= false; this->my_Policy->reset_policy();}
    const string& get_name_id(){ return my_id;}
    Agent(Point* pos,Point* speed,string m_id , char m_team,int b_budget);
    Agent(Point* pos,Point* speed, char m_team,int b_budget);
    string get_id(){ return my_id; }
    char get_team() { return my_team; }
    string get_name();
    const Policy* getPolicy(){ return my_Policy;};
    void doAction(State *s);
    void setPolicy(Policy* pPtr){pPtr->set_id(this->my_id); this->my_Policy=pPtr;}
    ~Agent();

    void set_speed(Point *cur_speed) {
        if (this->my_speed != nullptr)
            delete(this->my_speed);
        this->my_speed=cur_speed;
    }

    void set_pos(Point *cur_pos) {
        if (this->my_pos != nullptr)
            delete(this->my_pos);
        this->my_pos=cur_pos;
    }
    int get_budget(){ return this->my_budget;}

    const Point* get_speed(){
        return this->my_speed;
    }

    const Point* get_pos(){
        return this->my_pos;
    }
    Point get_pos_v1(){
        return *my_pos;
    }
    Point get_speed_v1(){ return *my_speed;}

    string to_str();
    void to_print();



};


#endif //RACING_CAR_AGENT_HPP
