//
// Created by ERANHER on 25.5.2020.
//
#define GOT_HERE std::cout << "At " __FILE__ ":" << __LINE__ << std::endl

#ifndef TRACK_RACING_REALTIMESIMULATION_HPP
#define TRACK_RACING_REALTIMESIMULATION_HPP
#include "headers/Abstract/Simulation.hpp"

class rtSimulation{

    /*
     *
     * Agent A with fully policy + real starting place
     * Grid with the real goals and real dimensions
     * Vector<RTDP> order by the ID mini-grids
     * Component that take a real-state -> abstract state with the ID min-grid
     * Component recognize when two agents are in the same mini grid
     * Score Tracking e.g. coll,wall ....
     * output the paths of the two agents
     *
     * */
    vector<u_int32_t> trackingData;
    Point abstraction;
    Point GridSize;
    string _nameA;
    string _nameD;
    u_int32_t curAgentNumber=-1;
    std::vector<shared_ptr<Agent>> lDefenderAgent;
    Agent* _attacker;
    Agent* _defender;
    u_int32_t iterMax=1000;
    State* state;
    //std::unique_ptr<Agent> _defender;

    State* GetActionAbstract(State *s){
        return s->getAbstractionState(abstraction);
    }
    u_int32_t getIndexMiniGrid(const Point &statePos){

        auto row = statePos.array[0]/abstraction.array[0];
        auto col = statePos.array[1]/abstraction.array[1];
        auto z = statePos.array[2]/abstraction.array[2];
        return (abstraction.array[0])*row+col+z*(abstraction.array[2]);
    }

    void checkMeeting()
    {
        u_int32_t ans = lDefenderAgent.size()-1;
        auto l = state->getAllPos(this->abstraction);
        for (int i=0;i<l.size();++i)
        {
            for(int j=i+1;j<l.size();++j)
                if(l[i]==l[j])
                {

                    ans = getIndexMiniGrid(l[i]);
                    break;
                }
        }
        if (ans != curAgentNumber)
        {
            curAgentNumber=ans;
            changeIDs();
        }
    }

public:
    rtSimulation(const Point& _abstraction,const Point& GridSize,std::vector<shared_ptr<Agent>> lD, Agent* attacker,State* stateArg,Agent* defnder)
    :abstraction(_abstraction),GridSize(GridSize),lDefenderAgent(std::move(lD)),
    _attacker(std::move(attacker)),state(std::move(stateArg)),_defender(std::move(defnder)),trackingData(event::Size,0)
    {
        curAgentNumber=lDefenderAgent.size()-1;
        changeIDs();

    }

    void changeIDs()
    {
        auto l = this->state->getIDs();
        for (auto &item:l)
        {
            if(item[item.size()-1]==Section::adversary)
            {
                this->_attacker->setID(item);
            }
            else this->lDefenderAgent[curAgentNumber]->setID(item);
        }
        this->lDefenderAgent[curAgentNumber]->evalPolicy();
    }

    void simulation()
    {
        for(size_t k=0;k<iterMax;++k)
        {
            bool stop = false;
            cout<<this->state->to_string_state()<<endl;
            while(!Stop_Game())
            {
                checkMeeting();
                if (curAgentNumber==lDefenderAgent.size()-1)
                {
                    auto tmpState = GetActionAbstract(this->state);
                    lDefenderAgent[curAgentNumber]->doAction(tmpState);
                    delete tmpState;
                    this->state->applyAction(lDefenderAgent[curAgentNumber]->get_id(),
                                             lDefenderAgent[curAgentNumber]->lastAction,
                                             lDefenderAgent[curAgentNumber]->getPolicyInt()->max_speed);
                } else
                    lDefenderAgent[curAgentNumber]->doAction(this->state);
                _attacker->doAction(this->state);

                cout<<this->state->to_string_state()<<endl;
            }
            reset_state();
        }
        cout<<"----"<<endl;
        printStat();
    }
    bool Stop_Game(){
        const Point& posEvader= this->state->get_position(this->_attacker->get_id());
        const Point& posPursuer = this->state->get_position(this->lDefenderAgent[curAgentNumber]->get_id());
        
        auto valGoal = state->g_grid->get_goal_reward(posEvader);
        if (state->g_grid->is_wall(posPursuer)) // agent P hit wall
        {
            trackingData[event::WallId]++;
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
        if(posEvader==posPursuer) // P and E coll
        {
            trackingData[event::CollId]++;
            return true;
        }
        return false;

    }
    void reset_state(){

        auto _rand = _attacker->getPolicyInt()->getRandom();
        auto [pPos,sSpeed] = _attacker->get_pos(_rand);
        this->state->set_position(_attacker->get_id(),
                                   pPos);
        this->state->set_speed(_attacker->get_id(),sSpeed);

        //auto _rand = _attacker->getPolicyInt()->getRandom();
        auto [DpPos,DsSpeed] = _defender->get_pos(_rand);
        this->state->set_position(_defender->get_id(),
                                  DpPos);
        this->state->set_speed(_defender->get_id(),DsSpeed);

    }
    void printStat()
    {
        //        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4
        cout<<"Coll: "<<this->trackingData[event::CollId]<<"\t";
        cout<<"Wall: "<<this->trackingData[event::WallId]<<"\t";
        cout<<"Goal: "<<this->trackingData[event::GoalId]<<"\t";
        cout<<"Open: "<<this->trackingData[event::OpenId]<<"\t";
        cout<<endl;

    }
};

#endif //TRACK_RACING_REALTIMESIMULATION_HPP
