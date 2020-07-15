
//
// Created by eranhe on 09/07/2020.
//

#ifndef TRACK_RACING_FIXSIMULATION_HPP
#define TRACK_RACING_FIXSIMULATION_HPP
#include "util_game.hpp"
#include "Policy.hpp"
#include "Agent.hpp"
#include "fixManager.hpp"
#define DEBUGING
class fixSimulation{

    //Grid _g;

    std::unique_ptr<Agent> _attacker;
    std::shared_ptr<Agent> _defender;
    std::unique_ptr<State> _state;
    fixManager _manager;

public:
    fixSimulation(configGame &conf,Policy *policyA,Policy *policyD,std::vector<weightedPosition>& listPointAttacker
    ,std::vector<weightedPosition>& listPointDefender,std::vector<pair<Point,Point>> &levels,State *s):
    _attacker(std::make_unique<Agent>(listPointAttacker,adversary,1)),
    _defender(std::make_unique<Agent>(listPointDefender,gurd,1)),
    _state(std::make_unique<State>(*s))
    {
        _attacker->setPolicy(policyA);
        _defender->setPolicy(policyD);
        _manager = fixManager(conf,levels,_defender,_state.get());
    }
    void main_loop()
    {
        while(true)
        {
            reset();
            while(true)
            {
                if(loop())
                    break;
            }
            this->_manager.end();
            if(is_converage())
                break;
        }
    }
    bool loop()
    {
        #ifdef DEBUGING
        cout<<_state->to_string_state()<<endl;
        #endif
        _manager.managing(_state.get());
        _manager.make_action(_state.get());
        _attacker->doAction(_state.get());
         return check_condtion();
    }
private:

    bool check_condtion()
    {
        const Point& pos_A = this->_state->get_position_ref(this->_attacker->get_id());
        const Point& pos_D = this->_state->get_position_ref(this->_defender->get_id());

        //wall
        is_absolut_wall(pos_D);
        //goal
        is_absolut_goal(pos_A);
        //coll
        is_absolut_collision(pos_D,pos_A);

    }

    inline static bool is_absolut_collision(const Point& pos_D,const Point& pos_A)
    {
        return pos_A==pos_D;
    }
    inline bool is_absolut_wall(const Point& pos_D)
    {
        return this->_state->g_grid->is_wall(pos_D);
    }
    inline bool is_absolut_goal(const Point& pos_A)
    {
        // res == 0 - means that the goal is with zero reward for D
        // res>0 - means that the goal is -reward for D agent
        return _state->g_grid->get_goal_reward(pos_A)>=0;
    }


    void reset()
    {
        _manager.reset();
        _attacker.reset();
    }
    bool is_converage() {return false;}
};


#endif //TRACK_RACING_FIXSIMULATION_HPP
