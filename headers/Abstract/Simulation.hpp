//
// Created by ERANHER on 13.5.2020.
//

//#define DEBUG3
//#define DEBUG2
#ifndef TRACK_RACING_SIMULATION_HPP
#define TRACK_RACING_SIMULATION_HPP
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

    Grid *g;
    bool noSpeed=false;
    State *sState;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    unordered_map<string,u_int32_t> collustionMap;

public:
    std::vector<shared_ptr<Agent>> agents;
    u_int16_t gridID;
    void getDefAgentDATA(){agents[0]->getPolicyInt()->policy_data();}
    shared_ptr<Agent> getDefAgent(){agents[0];}
    Agent* getDefAgentPTR(){agents.operator[](0).get();}
    size_t getCollustionMapSize(){return collustionMap.size();}
    const unordered_map<string,u_int32_t>& getCollustionMap()const{return collustionMap;}
    simulation(Agent* pursuerAgent, Agent* evaderAgent, Grid *absGrid,int seed,int _id)
    :trackingData(event::Size),distribution(0.0,1.0),generator(seed),sState(nullptr),
    g(absGrid)
    {
        gridID=_id;
        agents.push_back(std::shared_ptr<Agent>(pursuerAgent));
        agents.push_back(std::shared_ptr<Agent>(evaderAgent));
        for(const auto &item:agents)
            item->print();
        setState();
    }
    bool isInCollMap(const string &key)
    {
        if(auto pos = collustionMap.find(key);pos==collustionMap.end())
            return false;
        return true;
    }
    void noSpeedAttacker()
    {
        noSpeed=true;
    }
    ~simulation(){
        cout<<"~simulation"<<endl;

        delete sState;
        delete g;
    }


    std::thread member1Thread(const u_int32_t iter) {
        return std::thread([=]{ simulate(iter); });
    }
    /**
     * Game simulator for two agents
     * :iterationMax = number of simulations
     * **/
    void simulate(u_int32_t iterationMax){

        for (u_int32_t i = 0; i < iterationMax; ++i) {
            //if(i%1000==0) cout<<i<<endl;
            auto stop= false;
            #ifdef DEBUG3
            cout<<sState->to_string_state()<<endl;
            #endif
            stop = checkCondition();
            while(!stop)
            {
                std::for_each(std::begin(agents),std::end(agents),
                    [&](const shared_ptr<Agent>& ptrAgent){
                        ptrAgent->doAction(sState);}
                    );
                #ifdef DEBUG3
                cout<<sState->to_string_state()<<endl;
                #endif
                stop = checkCondition();
            }
            reset_state();
            #ifdef DEBUG2
            if(i%10000==0) cout<<"Iter:\t"<<i<<endl;
            #endif
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
            insetToCollusionMap(posEvader);
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
    void insetToCollusionMap(const Point &p)
    {
        std::string strPoint = p.to_str();
        if(auto pos = collustionMap.find(strPoint);pos==collustionMap.end())
        {
            collustionMap.insert({strPoint,1});
        }
        else{ pos->second++;}
    }

};


#endif //TRACK_RACING_SIMULATION_HPP
