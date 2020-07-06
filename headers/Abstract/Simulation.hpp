//
// Created by ERANHER on 13.5.2020.
//
////

//#define DEBUGPrint
#define ConVer
#define DEBUG2
#define ASSERT
#ifndef TRACK_RACING_SIMULATION_HPP
#define TRACK_RACING_SIMULATION_HPP
#include <thread>
#include "util_game.hpp"
#include "Policy/Policy.hpp"
//#include "Policy/RTDP_util.hpp"
#include "Policy/RtdpAlgo.hpp"
#include "Policy/PathPolicy.hpp"
#include "Agent.hpp"
#include "Grid.hpp"
//pursuer and evader
#define DATA_P
template<size_t N,typename V = u_int64_t>
struct Converager{
    std::array<V,N> arr_con = std::array<V,N>();
    size_t ctr=0;

    void inset_elm(V&& v)
    {
        //std::for_each(arr_con.begin(),arr_con.end(),[](auto x){cout<<x<<",";});
        //cout<<endl;
        arr_con[ctr]=std::forward<V>(v);
        ctr = ++ctr%N;
    }
    bool is_converage()
    {
        auto size_ctr = N;
        while(--size_ctr>0 && arr_con[size_ctr]==arr_con[0]);
        return size_ctr==0;
    }


};


namespace event{
    enum event : short{
        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4,
    };
    enum agnetIDX{defenderInt=0,attackerInt=1};


}


class simulation{

    vector<u_int32_t> trackingData;
    std::unique_ptr<Grid> g;
    bool noSpeed=false;
    int _seed;
    std::unique_ptr<State> sState;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    unordered_map<string,u_int32_t> collustionMap;
    Converager<15> arr_converage;
    u_int32_t ctr_converage = 0;
    const u_int32_t FixInset = 80000;
    #ifdef DATA_P
    Converager<5,std::vector<uint32_t >> evalPolicyer;
    vector<u_int32_t> acc_dataTrack = std::vector<u_int32_t>(event::Size);
    u_int64_t ctrPolicyEvla=0;
    #endif
public:
    vector<u_int32_t > getAverageInfo()
    {
        this->evalPolicyer.arr_con.back();
    }
    std::vector<shared_ptr<Agent>> agents;
    u_int16_t gridID;

    simulation(simulation&& obj) noexcept
    :trackingData(std::move(obj.trackingData)),g(std::move(obj.g)),
        noSpeed(obj.noSpeed),_seed(obj._seed),
        sState(std::move(obj.sState)),
        generator(obj.generator),distribution(obj.distribution),
        collustionMap(std::move(obj.collustionMap)),
        agents(std::move(obj.agents)),gridID(obj.gridID)
        {}

    simulation& operator=(const simulation& obj) noexcept
    {
        this->trackingData=(obj.trackingData);
        this->sState=std::make_unique<State>(*obj.sState);
        this->g=std::make_unique<Grid>(*obj.g);
        this->generator= obj.generator;
        this->distribution=obj.distribution;
        this->collustionMap= (obj.collustionMap);
        this->noSpeed=obj.noSpeed;
        this->gridID = obj.gridID;
        this->agents = (obj.agents);
        this->_seed=obj._seed;
        return *this;
    }

    void print_evalPolicyer()
    {
        for(const auto &item:this->evalPolicyer.arr_con)
        {
            for(const auto item1:item) cout<<item1<<",";
            cout<<endl;
        }
    }
    void getDefAgentDATA(){agents[event::agnetIDX::defenderInt]->getPolicyInt()->policy_data();}
    shared_ptr<Agent> getDefAgent(){agents[event::agnetIDX::defenderInt];}
    Agent* getDefAgentPTR(){agents.operator[](0).get();}
    size_t getCollustionMapSize(){return collustionMap.size();}
    const unordered_map<string,u_int32_t>& getCollustionMap()const{return collustionMap;}
    simulation(Agent* pursuerAgent, Agent* evaderAgent, std::unique_ptr<Grid>  absGrid,int seed,int _id)
    :trackingData(event::Size),_seed(seed),distribution(0.0,1.0),generator(seed),sState(nullptr),
    g(std::move(absGrid))
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
        auto DefnderEntryPoints = agents[event::agnetIDX::defenderInt]->getAllPositions();
        auto AttackerEntryPoints = agents[event::agnetIDX::attackerInt]->getAllPositions();
        for( const auto &item:DefnderEntryPoints)
        {
            setPosSpeed(item.speedPoint,item.positionPoint,agents[event::agnetIDX::defenderInt]->get_id());
            auto w = item.weightedVal;
            for(const auto &attackerPointItem:AttackerEntryPoints )
            {
                setPosSpeed(attackerPointItem.speedPoint,attackerPointItem.positionPoint,agents[event::agnetIDX::attackerInt]->get_id());
                w*=attackerPointItem.weightedVal;
                auto ptrPolicy= agents[event::agnetIDX::defenderInt]->getPolicyInt();
                auto *tmp_pointer = dynamic_cast <RtdpAlgo*>(ptrPolicy);
                double val = tmp_pointer->getArgMaxValueState(sState.get());
                #ifdef ASSERT
                if(val > 1.0 or val < -10.0)
                {
                    cout<<"val: "<<val<<endl;
                    assert(false);
                }
                #endif
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
    }


    std::thread member1Thread(const u_int32_t iter) {
        return std::thread([=]{ simulate(iter); });
    }
    /**
     * Game simulator for two agents
     * :iterationMax = number of simulations
     * **/
    void simulate(u_int32_t iterationMax){
        bool stop=false;
        #ifdef DEBUG2
        cout<<"---> gridID:"<<gridID<<endl;
        #endif
        for (u_int32_t i = 0; i < iterationMax; ++i) {
            if(stop)
                break;
            #ifdef DEBUGPrint
            cout<<sState->to_string_state()<<endl;
            #endif
            if(checkCondition()) // if the 1st state is terminal
            {
                std::for_each(agents.begin(),agents.end(),
                              [&](const shared_ptr<Agent>& ptrAgentI)
                              {
                    ptrAgentI->getPolicyInt()->update_final_state(sState.get());
                              });
            }
            else {
                while (true) {
                    std::for_each(std::begin(agents), std::end(agents),
                                  [&](const shared_ptr<Agent> &ptrAgent) {
                                      ptrAgent->doAction(sState.get());
                                  }
                    );
                    #ifdef DEBUGPrint
                    cout<<sState->to_string_state()<<endl;
                    #endif
                    #ifdef ConVer
                    if (isConverage(agents[event::agnetIDX::defenderInt].get())) stop=true;
                    #endif
                    if (checkCondition()) break;
                }
            }
            #ifdef DEBUGPrint
            cout<<"END"<<endl;
            #endif
            setPolicyEval();
            reset_state();
            #ifdef DEBUG2
            if(i%1000000==0) {cout<<"Iter:\t"<<i<<endl;printMe();}
            #endif
        }
        print_evalPolicyer();
        printStat();
    }

    RtdpAlgo* getRtdpAlgo()
    {
        return dynamic_cast<RtdpAlgo*>(this->agents[event::agnetIDX::defenderInt]->getPolicyInt());
    }

private:
    void printMe(){

        cout<<"Coll: "<<this->trackingData[event::CollId]<<"\t";
        cout<<"Wall: "<<this->trackingData[event::WallId]<<"\t";
        cout<<"Goal: "<<this->trackingData[event::GoalId]<<"\t";
        cout<<"Open: "<<this->trackingData[event::OpenId]<<"\t";
        cout<<endl;
    }
    void printStat()
    {
        //        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4
        cout<<"sim out ID-->"<<this->gridID<<endl;
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
        auto pathPoly = dynamic_cast <PathPolicy*>(agents[event::agnetIDX::attackerInt]->getPolicyInt());
        cout<<"size Dict Attacker: "<<pathPoly->get_dictPolicy_size()<<"\n";
        getRtdpAlgo()->genrateInfoPrint();
        //getDefAgentDATA();

    }
    void setState()
    {
        sState = std::make_unique<State>();
        sState->g_grid=g.get();
        for(auto& itemAgent:agents)
        {
            auto [pPos,sSpeed] = itemAgent->get_pos(this->distribution(this->generator));
            sState->add_player_state(itemAgent->get_id(),pPos,sSpeed
                                     ,itemAgent->get_budget());
        }

    }
    bool isConverage(const Agent *a)
    {
        ctr_converage++;
        if(ctr_converage%FixInset==0)
        {
            ctr_converage=0;
            const auto* rtdp_ptr = dynamic_cast<const RtdpAlgo*>(a->getPolicy());
            arr_converage.inset_elm(rtdp_ptr->getUpdateCtr());
            return arr_converage.is_converage();
        }
        return false;
    }
    /**
     * Reset the State
     * **/
    void reset_state(){
        for(auto& itemAgent:agents)
        {
            auto [pPos,sSpeed] = itemAgent->get_pos(this->distribution(this->generator));
            setPosSpeed(sSpeed,pPos,itemAgent->get_id());
            itemAgent->rest();
        }

    }
    void setPolicyEval()
    {
        ctrPolicyEvla++;
        if((ctrPolicyEvla%10000)!=0) return;
        vector<u_int32_t> l(event::Size);
        for(auto i = 0 ; i<event::Size;++i)
            l[i]=trackingData[i]-acc_dataTrack[i];
        std::copy(this->trackingData.begin(),this->trackingData.end(),acc_dataTrack.begin());
        evalPolicyer.inset_elm(std::move(l));

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
