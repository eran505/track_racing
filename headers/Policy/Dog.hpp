//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_DOG_HPP
#define RACING_CAR_DOG_HPP

#include <utility>

#include "Policy.hpp"

class Dog: public Policy{
    int x;

public:
    Dog(string namePolicy, int speed_MAX, int x);
    Dog(string namePolicy,int speed_MAX);
    void set_move(int x);
    Point get_action(State *s) override;
    ~Dog();
    void set_x(int dest){this->x=dest;}
    void reset_policy() override;
    void policy_data() override;

};

Point Dog::get_action(State *s){
    //auto x = range_random(-1,1);
    //auto y = range_random(-1,1);

    Point p(0,0);
    return p;
}

void Dog::set_move(int x) {
    this->x=x;
}

void Dog::reset_policy() {
    Policy::reset_policy();
    printf("Dog::reset");
}

void Dog::policy_data() {
    Policy::policy_data();
    printf("Dog::No Data is available");
}

Dog::Dog( string namePolicy,int MAX_SPEED) : Policy( std::move(namePolicy),MAX_SPEED) {
    this->x=1;
}

Dog::~Dog() {
    //cout<<"DOG del"<<endl;
}

Dog::Dog(string namePolicy, int speed_MAX,int x) : Policy(std::move(namePolicy),speed_MAX) {
    this->x=x;
}

#endif //RACING_CAR_DOG_HPP
