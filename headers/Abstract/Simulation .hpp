//
// Created by ERANHER on 13.5.2020.
//

#ifndef TRACK_RACING_SIMULATION_HPP
#define TRACK_RACING_SIMULATION_HPP

#include "util_game.hpp"
#include "Policy/Policy.hpp"
#include "Agent.hpp"
#include "Grid.hpp"
//pursuer and evader
class simulation{

    std::unique_ptr<Agent> pursuer;
    std::unique_ptr<Agent> evader;
    Grid* g;
    State sState;
    simulation(std::unique_ptr<Agent> pursuerAgent, std::unique_ptr<Agent> evaderAgent, Grid *absGrid )
    {
        g=absGrid;
        pursuer=std::move(pursuerAgent);
        evader=std::move(evaderAgent);

    }
    ~simulation(){
        delete (g);
    }
    void simulate(u_int32_t iteration){

    }
    /**
     * 1) wall
     * 2) out budget
     * 3) hit
     * 4) at goal
     * **/
    bool checkCondition(){
        const Point& posPursuer = this->sState.get_position(this->pursuer->get_id());
        if (g->is_at_goal(posPursuer))
        {
            //the evader is at a goal
            return true;
        }
        const Point& posEvader = this->sState.get_position(this->evader->get_id());
        if (g->is_wall(posEvader))
        {
            return true;
        }

    }


};


#endif //TRACK_RACING_SIMULATION_HPP
