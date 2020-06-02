//
// Created by ERANHER on 25.5.2020.
//
#define GOT_HERE std::cout << "At " __FILE__ ":" << __LINE__ << std::endl
#define DEBUGER
//#define PRINTME
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
    vector<float> trackingData;
    Point abstraction;
    Point GridSize;
    bool inMini=false;
    set<string> collPoints;
    set<string> wallPoints;
    string _nameA;
    u_int32_t sizeM;
    string _nameD;
    u_int32_t curAgentNumber=-1;
    unordered_map<u_int32_t ,Agent*> lDefenderAgent;
    Agent* _attacker;
    Agent* _defender;
    u_int32_t iterMax=100000;
    unordered_map<int,float > collusionMiniGrid;
    State* state;
    //std::unique_ptr<Agent> _defender;

    State* GetActionAbstract(State *s){
        return s->getAbstractionState(abstraction);
    }
    u_int32_t getIndexMiniGrid(const Point &statePos){

        auto row = statePos.array[0]*abstraction.array[0];
        auto col = statePos.array[1]%abstraction.array[1];
        return col+row;

    }

    void checkMeeting()
    {
        u_int32_t ans = sizeM;
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
            #ifdef DEBUGER
            if(curAgentNumber<sizeM)
                insetCollDict();
            #endif
            changeIDs();
        }
    }

public:
    rtSimulation(const Point& _abstraction,const Point& GridSize,unordered_map<u_int32_t ,Agent*> lD, Agent* attacker,State* stateArg,Agent* defnder)
    :abstraction(_abstraction),GridSize(GridSize),lDefenderAgent(std::move(lD)),
    _attacker(std::move(attacker)),state(std::move(stateArg)),_defender(std::move(defnder)),trackingData(event::Size,0)
    {
        sizeM = (this->GridSize/this->abstraction).accMulti();
        curAgentNumber=sizeM;
        changeIDs();

    }

    void changeIDs()
    {
        this->getAgent(curAgentNumber)->evalPolicy();
        return;
        auto l = this->state->getIDs();
        for (auto &item:l)
        {
            if(item[item.size()-1]==Section::adversary)
            {
                this->_attacker->setID(item);
            }
            else this->getAgent(curAgentNumber)->setID(item);
        }

    }

    void simulation()
    {
        for(size_t k=0;k<iterMax;++k)
        {
            u_int16_t ctr=0;
            #ifdef PRINTME
            cout<<ctr<<":\t"<<this->state->to_string_state()<<endl;
            #endif
            inMini=false;
            while(!Stop_Game())
            {
                ctr++;
                checkMeeting();
                if (curAgentNumber==sizeM)
                {
                    auto tmpState = GetActionAbstract(this->state);
                    auto ptrAgent = getAgent(curAgentNumber);
                    ptrAgent->doAction(tmpState);
                    delete tmpState;
                    this->state->applyAction(ptrAgent->get_id(),
                                             ptrAgent->lastAction,
                                             ptrAgent->getPolicyInt()->max_speed);
                } else
                    {
                        getAgent(curAgentNumber)->doAction(this->state);
                        inMini=true;
                        #ifdef PRINTME
                        cout<<curAgentNumber<<" ->  ";
                        #endif
                    }
                _attacker->doAction(this->state);
                #ifdef PRINTME
                cout<<ctr<<":\t"<<this->state->to_string_state()<<endl;
                #endif
            }
            reset_state();
        }
        cout<<"----"<<endl;
        #ifdef DEBUGER
        printCollDict();
        #endif
        printStat();
    }
    bool Stop_Game(){
        const Point& posEvader= this->state->get_position(this->_attacker->get_id());
        const Point& posPursuer = this->state->get_position(this->lDefenderAgent[curAgentNumber]->get_id());
        
        auto valGoal = state->g_grid->get_goal_reward(posEvader);
        if (state->g_grid->is_wall(posPursuer)) // agent P hit wall
        {
            wallPoints.insert(posPursuer.to_str());
            if(inMini)
                trackingData[event::OpenId]++;
            else
                {
                    trackingData[event::WallId]++;
                }
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
            collPoints.insert(posPursuer.to_str());
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
        cout<<"---Coll Points--\n"<<endl;
        std::for_each(collPoints.begin(),collPoints.end(),[](auto &strX){cout<<strX<<"\t";});

        cout<<"\n---Wall Points--\n"<<endl;
        std::for_each(wallPoints.begin(),wallPoints.end(),[](auto &strX){cout<<strX<<"\t";});

    }
    void insetCollDict()
    {
        if(auto pos = collusionMiniGrid.find(curAgentNumber);pos==collusionMiniGrid.end())
        {
            collusionMiniGrid.insert({curAgentNumber,1});
        }
        else{ pos->second++;}
    }
    void printCollDict(){
        for(auto &item: collusionMiniGrid){
            cout<<item.first<<" : "<<item.second<<endl;
        }
    }
    Agent * getAgent(unsigned int key)
    {

        auto pos = this->lDefenderAgent.find(key);
        if (pos==lDefenderAgent.end())
            throw ;
        return pos->second;
    }
};

#endif //TRACK_RACING_REALTIMESIMULATION_HPP
