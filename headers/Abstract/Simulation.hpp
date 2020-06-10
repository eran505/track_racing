//
// Created by ERANHER on 13.5.2020.
//
////
//#define DEBUGPrint
//#define DEBUG2
#ifndef TRACK_RACING_SIMULATION_HPP
#define TRACK_RACING_SIMULATION_HPP
#include <thread>
#include "util_game.hpp"
#include "Policy/Policy.hpp"
#include "Policy/RTDP_util.hpp"
#include "Policy/RtdpAlgo.hpp"
#include "Policy/PathPolicy.hpp"
#include "Agent.hpp"
#include "Grid.hpp"
//pursuer and evader

namespace event{
    enum event : short{
        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4
    };

}

class simulation{
    enum angentEntry{defenderInt=0,attackerInt=1};
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
    void getDefAgentDATA(){agents[defenderInt]->getPolicyInt()->policy_data();}
    shared_ptr<Agent> getDefAgent(){agents[defenderInt];}
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
    double getAvgExpectedReward()
    {
        double ans=0;
        auto DefnderEntryPoints = agents[defenderInt]->getAllPositions();
        auto AttackerEntryPoints = agents[attackerInt]->getAllPositions();
        for( const auto &item:DefnderEntryPoints)
        {
            setPosSpeed(item.speedPoint,item.positionPoint,agents[defenderInt]->get_id());
            auto w = item.weightedVal;
            for(const auto &attackerPointItem:AttackerEntryPoints )
            {
                setPosSpeed(attackerPointItem.speedPoint,attackerPointItem.positionPoint,agents[attackerInt]->get_id());
                w*=attackerPointItem.weightedVal;
                auto ptrPolicy= agents[defenderInt]->getPolicyInt();
                auto *tmp_pointer = dynamic_cast <RtdpAlgo*>(ptrPolicy);
                auto val = tmp_pointer->getArgMaxValueState(sState);
                ans += val*w;
            }
        }
        return ans;

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
            auto stop= false;
            #ifdef DEBUGPrint
            cout<<sState->to_string_state()<<endl;
            #endif
            stop = checkCondition();
            if(stop) // if the 1st state is terminal
            {
                std::for_each(agents.begin(),agents.end(),
                              [&](const shared_ptr<Agent>& ptrAgentI)
                              {
                    ptrAgentI->getPolicyInt()->update_final_state(sState);
                              });
            }
            while(!stop)
            {
                std::for_each(std::begin(agents),std::end(agents),
                    [&](const shared_ptr<Agent>& ptrAgent){
                        ptrAgent->doAction(sState);}
                    );
                #ifdef DEBUGPrint
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

    RtdpAlgo* getRtdpAlgo()
    {
        return dynamic_cast<RtdpAlgo*>(this->agents[defenderInt]->getPolicyInt());
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
        auto res = g->getAllGoalsData();
        std::for_each(res.begin(),res.end(),[&](pair<double,Point> &l){
            auto &[val,pos]=l;
            cout<<"Goal:"<<pos.to_str()<<"\tval:\t"<<val<<endl;
        });
        auto pathPoly = dynamic_cast <PathPolicy*>(agents[attackerInt]->getPolicyInt());
        cout<<"size Dict Attacker: "<<pathPoly->get_dictPolicy_size()<<"\n";
        getRtdpAlgo()->genrateInfoPrint();
        //getDefAgentDATA();

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
            setPosSpeed(sSpeed,pPos,itemAgent->get_id());
        }
    }
    void setPosSpeed(const Point &sSpeed,const Point &pPos,const string &id_str)
    {
        this->sState->set_position(id_str,
                                   pPos);
        this->sState->set_speed(id_str,sSpeed);
    }
    /**
     * 1) wall
     * 2) out budget (not implement)
     * 3) hit
     * 4) at goal
     * **/
    bool checkCondition(){
        const Point& posPursuer = this->sState->get_position_ref(this->agents[0]->get_id());
        const Point& posEvader = this->sState->get_position_ref(this->agents[1]->get_id());
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
