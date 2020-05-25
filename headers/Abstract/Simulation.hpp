//
// Created by ERANHER on 13.5.2020.
//

#define DEBUG
#ifndef TRACK_RACING_SIMULATION_HPP
#define TRACK_RACING_SIMULATION_HPP
#include "headers/Abstract/StateStoch.h"
#include <thread>
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
template <typename T>
class simulation{
    vector<u_int32_t> trackingData;
    std::vector<Agent*> agents;
    Grid *g;
    State *sState;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;

public:
    simulation(Agent* pursuerAgent, Agent* evaderAgent, Grid *absGrid,int seed)
    :trackingData(event::Size),distribution(0.0,1.0),generator(seed),sState(nullptr),
    g(absGrid)
    {
        agents.push_back(pursuerAgent);
        agents.push_back(evaderAgent);
        for(const auto &item:agents)
            item->print();
        setState();
    }
    void stoState(float x)
    {
        delete this->sState;
        this->sState=(State*)new stochasticState(x);
        setState();
    }
    ~simulation(){
        cout<<"~simulation"<<endl;
        for(auto &agentI:agents)
            delete agentI;
        delete g;
        delete sState;
    }
    void operator=(const simulation &other)
    {
        g=other.g;
        for(auto &agentI:agents)
            delete agentI;
        trackingData=other.trackingData;
        agents.clear();
        agents.reserve(other.agents.size());
        for(auto agentI:other.agents)
             agents.emplace_back(agentI);
        sState=other.sState;
        generator=other.generator;
        distribution=other.distribution;
    }

    std::thread member1Thread(const u_int32_t iter) {
        return std::thread([=] { simulate(iter); });
    }
    /**
     * Game simulator for two agents
     * :iterationMax = number of simulations
     * **/
    void simulate(u_int32_t iterationMax){
        for (u_int32_t i = 0; i < iterationMax; ++i) {
            //if(i%1000==0) cout<<i<<endl;
            auto stop= false;
            #ifdef DEBUG
            cout<<sState->to_string_state()<<endl;
            #endif
            stop = checkCondition();
            while(!stop)
            {
                std::for_each(std::begin(agents),std::end(agents),
                    [&](Agent* ptrAgent){
                        ptrAgent->doAction(sState);}
                    );
                #ifdef DEBUG
                cout<<sState->to_string_state()<<endl;
                #endif
                stop = checkCondition();
            }
            reset_state();
        }
        printStat();
    }

private:
    void printStat()
    {
        //        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4
        cout<<"Coll: "<<this->trackingData[event::CollId]<<"\t";
        cout<<"Wall: "<<this->trackingData[event::WallId]<<"\t";
        cout<<"Goal: "<<this->trackingData[event::GoalId]<<"\t";
        cout<<"Open: "<<this->trackingData[event::OpenId]<<"\t";
        cout<<endl;

    }
    void setState()
    {
        sState = new State();
        sState->g_grid=g;
        for(auto& itemAgent:agents)
        {
            auto [pPos,sSpeed] = itemAgent->get_pos(this->distribution(this->generator));
            sState->add_player_state(itemAgent->get_id(),pPos,sSpeed
                                     ,itemAgent->get_budget());
        }

    }


    /**
     * Reset the State
     * **/
    void reset_state(){
        for(auto& itemAgent:agents)
        {
            auto [pPos,sSpeed] = itemAgent->get_pos(this->distribution(this->generator));
            this->sState->set_position(itemAgent->get_id(),
                                       pPos);
            this->sState->set_speed(itemAgent->get_id(),sSpeed);
        }
    }

    /**
     * 1) wall
     * 2) out budget (not implement)
     * 3) hit
     * 4) at goal
     * **/
    bool checkCondition(){
        const Point& posPursuer = this->sState->get_position(this->agents[0]->get_id());
        const Point& posEvader = this->sState->get_position(this->agents[1]->get_id());
        auto valGoal = g->get_goal_reward(posEvader);
        if (g->is_wall(posPursuer)) // agent P hit wall
        {
            trackingData[event::WallId]++;
            return true;
        }
        if(posEvader==posPursuer) // P and E coll
        {
            trackingData[event::CollId]++;
            return true;
        }
        if (valGoal>=0) // E at a goal
        {
            if (valGoal==0)
                trackingData[event::OpenId]++;
            else
                trackingData[event::GoalId]++;
            return true;
        }
        return false;

    }
};


#endif //TRACK_RACING_SIMULATION_HPP
