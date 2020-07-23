//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_DOG_HPP
#define RACING_CAR_DOG_HPP

#include <utility>

#include "Policy.hpp"
typedef shared_ptr<unordered_map<string,string>> dictionary;

class Dog: public Policy{
    int x;
    vector<Point> golazz;
    int get_goal_point();
public:
    Dog(string namePolicy, int speed_MAX, int x,string agentID,string &home,dictionary ptrDict);
    Dog(string namePolicy,int MAX_SPEED,string agentID,string &home,dictionary ptrDict);
    void set_goal(Point p)
    {
        this->golazz.push_back(std::move(p));
    }

    Point get_action(State *s) override;
    ~Dog() override;
    void set_x(int dest){this->x=dest;}
    void reset_policy() override;
    void policy_data() const override;
    std::vector<double>* TransitionAction(const State*) override;

    Point clac_diff(int index_gaol,const Point &my_pos){
        auto goal_i = this->golazz[index_gaol];
        Point p(0,0);
        for (int i = 0; i < my_pos.capacity; ++i) {
            p.array[i] = goal_i.array[i]-my_pos.array[i];
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
    auto my_pos = s->get_position_ref(this->id_agent);
    auto my_speed = s->get_speed(this->id_agent);
    auto index = this->get_goal_point();
    auto goal_i = this->golazz[index];
    Point p;
    for (int i = 0; i < my_pos.capacity; ++i) {
        p.array[i] = goal_i.array[i] - (my_pos.array[i]+my_speed[i]);
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

void Dog::policy_data() const {
    printf("Dog::No Data is available");
}

Dog::Dog( string namePolicy,int MAX_SPEED,string agentID,string &home,dictionary ptrDict) : Policy( std::move(namePolicy),MAX_SPEED,std::move(agentID),home,ptrDict) {
    this->x=1;
}

Dog::~Dog() {
    cout<<"del DOG"<<endl;
}

Dog::Dog(string namePolicy, int speed_MAX,int x,string agentID,string &home,dictionary ptrDict) : Policy(std::move(namePolicy),speed_MAX,agentID,home, ptrDict) {
    this->x=x;

}

std::vector<double> *Dog::TransitionAction(const State *s) {

    auto l = new std::vector<double>();
    double prob = 1/double(this->golazz.size());
    for (int i = 0; i < golazz.size(); ++i) {
        auto action_i = clac_diff(i,s->get_position_ref(this->id_agent));
        auto action_index = action_i.hashMeAction(Point::actionMax);
        auto index = distance(l->begin(), find(l->begin(), l->end(), action_index));
        if (index==l->size()){
            l->push_back(action_index);
            l->push_back(prob);
        }
        else {
            l->operator[](index+1)+=prob;
        }


    }
    return l;
}

int Dog::get_goal_point() {
    return range_random(0,this->golazz.size()-1);
}




#endif //RACING_CAR_DOG_HPP
