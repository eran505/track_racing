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
    const char my_team;
    const string my_id;
    string name;


public:
    const string* get_name_id(){ return &my_id;}
    Agent(Point* pos,Point* speed,string m_id , char m_team,int b_budget);
    Agent(Point* pos,Point* speed, char m_team,int b_budget);
    string get_id(){ return my_id; }
    char get_team() { return my_team; }
    string get_name();
    const Policy* get_Policy(){ return my_Policy;};
    void do_action(State *s);
    void set_policy(Policy* p_ptr){p_ptr->set_id(this->my_id); this->my_Policy=p_ptr;}
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

    Point* get_speed(){
        Point *p = new Point(*this->my_speed);
        return p;
    }

    Point* get_pos(){
        Point *p = new Point(*this->my_pos);
        return p;
    }
    Point get_pos_v1(){
        return *my_pos;
    }
    Point get_speed_v1(){ return *my_speed;}

    string to_str();
    string to_print();



};


#endif //RACING_CAR_AGENT_HPP
