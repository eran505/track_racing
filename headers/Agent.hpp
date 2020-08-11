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
typedef std::vector<std::pair<double,Point>> weightedPointVector;
class Agent{

protected:
    Policy* my_Policy= nullptr;
    weightedPositionVector initialPosition;
    int my_budget;
    bool is_wall;
    char my_team;
    string my_id;
    string name;
    bool eval;

public:
    [[nodiscard]] const weightedPositionVector& getAllPositions()const{return initialPosition;}
    Point lastAction;
    [[nodiscard]] weightedPositionVector getAllPositions_copy()const{return initialPosition;}
    void setID(string &_id)
    {
        this->my_id=_id;
        this->my_Policy->set_id(_id);
        this->my_team=_id[_id.size()-1];
    }
    int get_max_speed()const{return this->my_Policy->max_speed;}
    [[nodiscard]] bool get_is_wall() const{ return is_wall;}
    void rest(){is_wall= false; this->my_Policy->reset_policy();}
    [[nodiscard]] const string& get_name_id()const{ return my_id;}
    Agent(weightedPositionVector Startpos,string m_id , char m_team,int b_budget);
    Agent(weightedPositionVector Startpos, char m_team,int b_budget);
    [[nodiscard]] const string& get_id()const{ return my_id; }
    [[nodiscard]] char get_team() const { return my_team; }
    string get_name();
    [[nodiscard]] const Policy* getPolicy()const{ return my_Policy;};
    void doAction(State *s);
    void setPolicy(Policy* pPtr){pPtr->set_id(this->my_id); this->my_Policy=pPtr;}
    ~Agent();
    void evalPolicy();
    void trainPolicy();

    Policy* getPolicyInt(){return my_Policy;}

    [[nodiscard]] int get_budget() const{ return this->my_budget;}

    [[nodiscard]] bool isAttacker() const
    {
        return this->get_team() == Section::adversary;
    }
    void print()
    {
    }
    [[nodiscard]] pair<const Point&,const Point&> get_pos(double seed)const{
        double acc = 0;
        u_int16_t ctr=0;
        for (auto const &item:initialPosition){
            acc += item.weightedVal;
            if (acc >= seed)
                return {item.positionPoint,item.speedPoint};
            ctr++;
        }
        // doubleing point problem
        return {initialPosition[initialPosition.size()-1].positionPoint,initialPosition[initialPosition.size()-1].speedPoint};
    }



    static int ctr_object;
};


#endif //RACING_CAR_AGENT_HPP
