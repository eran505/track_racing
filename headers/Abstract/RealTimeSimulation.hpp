//
// Created by ERANHER on 25.5.2020.
//
#define GOT_HERE std::cout << "At " __FILE__ ":" << __LINE__ << std::endl
#define DEBUGER
#define PRINTME
#ifndef TRACK_RACING_REALTIMESIMULATION_HPP
#define TRACK_RACING_REALTIMESIMULATION_HPP
#include "headers/Abstract/Simulation.hpp"
#include "containerAbstract.h"
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
    vector<double> trackingData;
    Point GridSize;
    bool inMini=false;
    string resultsPath;
    set<string> collPoints;
    set<string> wallPoints;
    const Point zeroPoint = Point(0);
    string _nameA;
    string _nameD;
    unordered_map<u_int32_t ,Agent*> lDefenderAgent;
    Agent* _attacker;
    Agent* _defender;
    u_int32_t iterMax=10000;
    unordered_map<string,u_int32_t > collusionMiniGrid;
    State* state;
    //std::unique_ptr<Agent> _defender;
    vector<containerAbstract> conL;
    size_t idxContier = 0;

    std::unique_ptr<State> GetActionAbstract(State *s){
        return s->getAbstractionState(conL[idxContier].get_absPoint());
    }
    u_int32_t getIndexMiniGrid(const Point &statePos){

        auto row = statePos.array[0]*conL[idxContier].get_divPoint()[0];
        auto col = statePos.array[1]%conL[idxContier].get_divPoint()[1];
        return col+row;

    }


    void checkIfStateInPolicy(State *s,const Agent* a)
    {
        bool isIn = a->getPolicy()->isInPolicy(s);
        if(!isIn)
            s->set_speed(a->get_id(),this->zeroPoint);
    }
public:
    unordered_map<string,u_int32_t > getCollusionMiniGrid(){return collusionMiniGrid;}
    vector<double> getTrackingData(){return trackingData;}
    vector<string> getTrackingDataString()
    {
        auto v = std::vector<string>();
        for(auto i : trackingData)
            v.push_back(std::to_string(i));
        return v;
    }
    rtSimulation(const Point& GridSize, Agent* attacker,State* stateArg,Agent* defnder)
    :GridSize(GridSize),
    _attacker(attacker),_defender(defnder),trackingData(event::Size,0),
    state(stateArg)
    {

    }
    static void evalMode(Agent* ptr)
    {
        ptr->evalPolicy();
    }
    string collusionMiniGrid_to_string(){
        string str;
        for(auto &pair: collusionMiniGrid)
        {
            str+=pair.first+":"+std::to_string(pair.second);
            str+=" | ";
        }
        return str;
    }
    void set_agent(unordered_map<u_int32_t,Agent*> &&map)
    {
        this->lDefenderAgent= std::move(map);
        for(auto &item:lDefenderAgent){item.second->evalPolicy();}
    }

    void setContiner(vector<containerAbstract> l)
    {
        conL = std::move(l);
        for(auto &item:conL){item.eval();}
    }
    void simulationV2()
    {
        for (size_t t=0 ; t<iterMax;++t)
        {
            simGame();
            reset_state();
        }
        cout<<"----"<<endl;
        printCollDict();
        printStat();
    }
    static bool in_distance(const Point& one,const Point& two)
    {

        if(std::abs(one[0]-two[0])>2)
            return false;
        if(std::abs(one[1]-two[1])>2)
            return false;
        if(std::abs(one[2]-two[2])>2)
            return false;
        return true;
    }
    void change_continer(const State* ptr){
        vector<Point> getPos;
        ptr->getAllPos(getPos,conL[idxContier].get_absPoint());
        #ifdef DEBUGER
        assert(getPos.size()==2);
        #endif
        if(in_distance(getPos[0],getPos[1]))
        {
            if(this->idxContier<this->conL.size()-1)
            {
                idxContier++;
                //cout<<"idxContier++\n";
            }
        }
    }
    /**
    * pre-condition: all agents need to be at eval mode
    **/
    void simGame() {
        inMini=false;
        size_t ctr=0;
        #ifdef PRINTME
        cout<<ctr<<":\t"<<this->state->to_string_state()<<endl;
        #endif
        while (true) {
            change_continer(this->state);
            #ifdef PRINTME
            //cout<<"change_continer:(idx)\t"<<idxContier<<endl;
            #endif
            ctr++;

            // get the right agent Defender
            auto agentD = conL[idxContier].get_agent(state);
            evalMode(agentD);

            bool abstractionMode = conL[idxContier].get_absState();

            #ifdef DEBUGER
            if(!abstractionMode)
                insetCollDict();
            #endif

            if (Stop_Game())
                break;


            // if need to abstract the state
            if (abstractionMode) {
                auto tmpState = GetActionAbstract(this->state);
                #ifdef PRINTME
                cout<<"tmpState:\t"<<tmpState->to_string_state()<<endl;
                #endif
                // check if the state in the policy else zero the speed
                checkIfStateInPolicy(tmpState.get(),agentD);

                agentD->doAction(tmpState.get());
                #ifdef PRINTME
                cout<<"\tAction:\t"<<agentD->lastAction.to_str()<<"\n";
                #endif
                this->state->applyAction(agentD->get_id(), agentD->lastAction,
                                         agentD->getPolicyInt()->max_speed);

            } else {
                agentD->doAction(state);
            }
            // attacker turn
            _attacker->doAction(this->state);
            #ifdef PRINTME
            cout<<ctr<<":\t"<<this->state->to_string_state()<<endl;
            #endif
        }
    }
    bool Stop_Game(){
        const Point& posEvader= this->state->get_position_ref(this->_attacker->get_id());
        const Point& posPursuer = this->state->get_position_ref(this->_defender->get_id());
        
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
        idxContier=0;
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
        if(inMini) // dont mark more than one time
            return;
        inMini= true;
        auto str_point = getAbstractPoint();
        if(auto pos = collusionMiniGrid.find(str_point);pos==collusionMiniGrid.end())
        {
            collusionMiniGrid.insert({str_point,1});
        }
        else{ pos->second++;}
    }
    string getAbstractPoint()
    {


        std::vector<Point> l;
        this->state->getAllPos(l,conL[idxContier].get_absPoint());
        assert(l.size()==2);
        return l.front().to_str();
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
