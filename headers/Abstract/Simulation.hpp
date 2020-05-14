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

namespace event{
    enum event : short{
        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4
    };
}

class simulation{
    vector<u_int32_t> trackingData;
    std::unique_ptr<Agent> pursuer;
    std::unique_ptr<Agent> evader;
    std::vector<std::unique_ptr<Agent>> agents;
    Grid* g;
    State *sState;

    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;

    simulation(std::unique_ptr<Agent> pursuerAgent, std::unique_ptr<Agent> evaderAgent, Grid *absGrid,int seed)
    :trackingData(event::Size),distribution(0.0,1.0),generator(seed),sState(nullptr),
    g(absGrid)
    {
        agents.push_back(std::move(pursuerAgent));
        agents.push_back(std::move(evaderAgent));
        setState();
    }
    void setState()
    {
        sState = new State();
        for(auto& itemAgent:agents)
        {
            sState->add_player_state(itemAgent->get_id(),itemAgent->get_pos(distribution(generator)),
                                     itemAgent->get_speed_v2(),itemAgent->get_budget());
        }

    }

    ~simulation(){
        delete (g);
        delete (sState);
    }
    /**
     * Reset the State
     * **/
    void reset_state(){
        for(auto& itemAgent:agents)
        {
            this->sState->set_position(itemAgent->get_id(),
                                       itemAgent->get_pos(distribution(generator)));
            this->sState->set_speed(itemAgent->get_id(),itemAgent->get_speed_v1());
        }
    }
    /**
     * Game simulator for two agents
     * :iterationMax = number of simulations
     * **/
    void simulate(u_int32_t iterationMax){
        for (u_int32_t i = 0; i < iterationMax; ++i) {
            auto stop= true;
            while(!stop)
            {
                std::for_each(std::begin(agents),std::end(agents),
                        [&](std::unique_ptr<Agent> ptrAgent){
                            ptrAgent->doAction(sState);}
                        );
                stop = checkCondition();
            }
            reset_state();
        }
    }
    /**
     * 1) wall
     * 2) out budget (not implement)
     * 3) hit
     * 4) at goal
     * **/
    bool checkCondition(){
        const Point& posPursuer = this->sState->get_position(this->pursuer->get_id());
        auto valGoal = g->get_goal_reward(posPursuer);
        if (valGoal>0) // E at a goal
        {
            if (valGoal==0)
                trackingData[event::OpenId]++;
            else
                trackingData[event::GoalId]++;
            return true;
        }
        const Point& posEvader = this->sState->get_position(this->evader->get_id());
        if (g->is_wall(posEvader)) // agent P hit wall
        {
            trackingData[event::WallId]++;
            return true;
        }
        if(posEvader==posPursuer) // P and E coll
        {
            trackingData[event::CollId]++;
            return true;
        }

        return false;

    }
};


#endif //TRACK_RACING_SIMULATION_HPP
