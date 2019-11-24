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
    Dog(string idName, string namePolicy);
    Dog(string idName, string namePolicy,int x);
    void set_move(int x);
    Point get_action(State s) override;
    ~Dog();
    void set_x(int dest){this->x=dest;}
    void reset_policy() override;
    void policy_data() override;

};

Point Dog::get_action(State s){
    int move = this->x;
    Point p(move,move);
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

Dog::Dog(string idName, string namePolicy) : Policy(idName, namePolicy) {
    this->x=1;
}

Dog::~Dog() {

}

Dog::Dog(string idName, string namePolicy, int x) : Policy(idName, namePolicy) {
    this->x=x;
}

#endif //RACING_CAR_DOG_HPP
