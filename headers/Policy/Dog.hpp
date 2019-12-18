//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_DOG_HPP
#define RACING_CAR_DOG_HPP

#include <utility>

#include "Policy.hpp"

class Dog: public Policy{
    int x;
    vector<pair<int,int>*> golazz;
    int get_goal_point();
public:
    Dog(string namePolicy, int speed_MAX, int x);
    Dog(string namePolicy,int MAX_SPEED);
    void set_goal(int x,int y)
    {
        this->golazz.push_back(new pair<int,int>(x,y));
    }

    Point get_action(State *s) override;
    ~Dog();
    void set_x(int dest){this->x=dest;}
    void reset_policy() override;
    void policy_data() override;
    std::vector<float>* TransitionAction(State*) override;

    Point clac_diff(int index_gaol,const Point &my_pos){
        auto goal_i = this->golazz[index_gaol];
        Point p(0,0);
        for (int i = 0; i < my_pos.capacity; ++i) {
            p.array[i] = goal_i->first-my_pos.array[i];
            if (p.array[i]>1)
                p.array[i]=1;
            else if (p.array[i]<-1)
                p.array[i]=-1;
        }
        return p;
    }
};



Point Dog::get_action(State *s){
//    auto x = range_random(0,0);
    auto my_pos = s->get_position(this->id_agent);
    auto index = this->get_goal_point();
    auto goal_i = this->golazz[index];
    Point p(0,0);
    for (int i = 0; i < my_pos.capacity; ++i) {
        p.array[i] = goal_i->first - my_pos.array[i];
        if (p.array[i]>1)
            p.array[i]=1;
        else if (p.array[i]<-1)
            p.array[i]=-1;
    }
    return p;
}


void Dog::reset_policy() {
    printf("Dog::reset");
}

void Dog::policy_data() {
    printf("Dog::No Data is available");
}

Dog::Dog( string namePolicy,int MAX_SPEED) : Policy( std::move(namePolicy),MAX_SPEED) {
    this->x=1;
}

Dog::~Dog() {
    cout<<"del DOG"<<endl;
    for (auto & i : this->golazz) {
        delete i;
    }
}

Dog::Dog(string namePolicy, int speed_MAX,int x) : Policy(std::move(namePolicy),speed_MAX) {
    this->x=x;

}

std::vector<float> *Dog::TransitionAction(State *s) {

    auto l = new std::vector<float>();
    float prob = 1/float(this->golazz.size());
    for (int i = 0; i < golazz.size(); ++i) {
        auto action_i = clac_diff(i,s->get_position(this->id_agent));
        l->push_back(this->get_hash_index(action_i));
        l->push_back(prob);
    }
    return l;
}

int Dog::get_goal_point() {
    return range_random(0,this->golazz.size()-1);
}

#endif //RACING_CAR_DOG_HPP
