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
typedef vector<weightedPosition> weightedPositionVector;
typedef std::vector<std::pair<float,Point>> weightedPointVector;
class Agent{

protected:
    Policy* my_Policy;
    weightedPositionVector initialPosition;
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
    Agent(weightedPositionVector Startpos,string m_id , char m_team,int b_budget);
    Agent(weightedPositionVector Startpos, char m_team,int b_budget);
    const string& get_id(){ return my_id; }
    char get_team() const { return my_team; }
    string get_name();
    const Policy* getPolicy(){ return my_Policy;};
    void doAction(State *s);
    void setPolicy(Policy* pPtr){pPtr->set_id(this->my_id); this->my_Policy=pPtr;}
    ~Agent();
    void evalPolicy();
    void trainPolicy();

    Policy* getPolicyInt(){return my_Policy;}

    int get_budget() const{ return this->my_budget;}


    void print()
    {
        cout<<"ID"<<this->get_id()<<endl;
        for(auto &item:initialPosition)
        {
            cout<<item.positionPoint.to_str()<<"\t";
        }
        cout<<endl;
    }
    pair<const Point&,const Point&> get_pos(float seed){
        float acc = 0;
        u_int16_t ctr=0;
        for (auto const &item:initialPosition){
            acc += item.weightedVal;
            if (acc >= seed)
                return {item.positionPoint,item.speedPoint};
            ctr++;
        }
        // floating point problem
        return {initialPosition[initialPosition.size()-1].positionPoint,initialPosition[initialPosition.size()-1].speedPoint};
    }


    static int ctr_object;
};


#endif //RACING_CAR_AGENT_HPP
