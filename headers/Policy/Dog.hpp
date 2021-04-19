//
// Created by ise on 19.11.2019.
//

#ifndef RACING_CAR_DOG_HPP
#define RACING_CAR_DOG_HPP

#include <utility>

#include "Policy.hpp"
typedef shared_ptr<unordered_map<string,string>> dictionary;

class Dog: public Policy{


    int x{};
    vector<Point> golazz;
    int get_goal_point();
    State::agentEnum attacker_ider = State::agentEnum::A;
    vector<vector<Point>> all_paths;
    vector<int32_t> rel_path;

public:
    Dog(int maxSpeedAgent, short agentId, string &home1, const string& namePolicy, int speed_MAX, int seed,
        const string& agentID, string &home);
    void set_goal(Point p)
    {
        this->golazz.push_back(std::move(p));
    }

    Point get_action(State *s) override;
    ~Dog() override;
    void set_x(int dest){this->x=dest;}
    void reset_policy() override;
    void policy_data() const override;
    std::vector<double>* TransitionAction(const State*) const override;

    [[nodiscard]] Point clac_diff(int index_gaol,const Point &my_pos)const{
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

    Point to_attacker(const State *s);

    Point to_go_goal(State *s);

    Dog(int speed_MAX, int x, short agentID, string &home);

    void inffer(const State *s);

    bool check_rel_path(const Point& a,int idx_path,int path_number);

    Point to_point(const Point &pos, const State *s);

    void set_all_paths(std::vector<std::vector<Point>> l)
    {
        this->all_paths=std::move(l);
    }
};

void Dog::inffer(const State *s)
{
    const auto& attack_point = s->get_position_ref(this->attacker_ider);
    std::vector<int> to_del;
    for (int i = 0; i < rel_path.size(); ++i) {
         bool is_rel = this->check_rel_path(attack_point,s->get_budget(this->attacker_ider),this->rel_path[i]);
         if(is_rel)
             to_del.push_back(this->rel_path[i]);


    }
    //cout<<to_del<<endl;
    if (to_del.size() != rel_path.size()) {
        rel_path.clear();
        std::copy(to_del.begin(), to_del.end(),
                  std::back_inserter(rel_path));
    }
}

Point Dog::get_action(State *s){
//    auto x = range_random(0,0);
    if(this->rel_path.size()==1)
        return this->to_point(this->all_paths[rel_path.front()][all_paths[rel_path.front()].size()-2],s);

    this->inffer(s);
    if(this->rel_path.size()==1)
        return this->to_point(this->all_paths[rel_path.front()][all_paths[rel_path.front()].size()-2],s);
    return Point(0);
}

Point Dog::to_point(const Point& pos,const State *s)
{
    auto my_pos = s->get_position_ref(this->id_agent);
    auto my_speed = s->get_speed(this->id_agent);
    Point p(0);
    for (int i = 0; i < my_pos.capacity; ++i) {
        p.array[i] = pos.array[i] - (my_pos.array[i]+my_speed[i]);
        if (p.array[i]>1)
            p.array[i]=1;
        else if (p.array[i]<-1)
            p.array[i]=-1;
    }
    return p;
}


Point Dog::to_attacker(const State *s){
//    auto x = range_random(0,0);
    auto my_pos = s->get_position_ref(this->id_agent);
    auto my_speed = s->get_speed(this->id_agent);
    auto index = this->get_goal_point();
    auto goal_i = s->get_position_ref(this->attacker_ider);
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

    rel_path.clear();
    rel_path.reserve(all_paths.size());
    for (int i = 0; i < all_paths.size(); ++i) {
        rel_path.push_back(i);
    }
}

void Dog::policy_data() const {
    printf("Dog::No Data is available");
}


Dog::~Dog() {
    cout<<"del DOG"<<endl;
}

Dog::Dog(int speed_MAX, int x, short agentID, string &home) : Policy(speed_MAX, agentID, home) {
    this->x=x;

}

std::vector<double> *Dog::TransitionAction(const State *s)const {

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

Dog::Dog(int maxSpeedAgent, short agentId, string &home1, const string& namePolicy, int speed_MAX, int seed,
         const string& agentID, string &home) : Policy(maxSpeedAgent, agentId, home1, seed) {

}

bool Dog::check_rel_path(const Point &a, int idx_path, int path_number) {

    for (int i = idx_path; i < this->all_paths[path_number].size(); ++i) {
        if ( a == this->all_paths[path_number][i] )
            return true;
    }
    return false;
}

Point Dog::to_go_goal(State *s) {
    return Point();
}


#endif //RACING_CAR_DOG_HPP
