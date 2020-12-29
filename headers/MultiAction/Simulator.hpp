//
// Created by eranhe on 27/07/2020.
//

#ifndef TRACK_RACING_SIMULATOR_HPP
#define TRACK_RACING_SIMULATOR_HPP


#include <headers/util/utilClass.hpp>
#include "util_game.hpp"
#include "Policy.hpp"
#include "Agent.hpp"
#include "util/saver.hpp"
#include "Scheduler.hpp"
#include "Policy/RtdpAlgo.hpp"
#include "Policy/RTDP_util.hpp"
#include "Policy/Attacker/PathFinder.hpp"
#define DEBUGING
//#define TRAJECTORY
#define Q_DATA
#define A_DATA

#define BUFFER_TRAJECTORY 1 // need to be 9000 when saving
#define STR_HOME_DIR "/car_model/out/"
#include "util/Rand.hpp"
#include "Abstract/Simulation.hpp"
namespace info
{
    enum info : short{
        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4,
    };
}
template<size_t N,typename V = u_int64_t>
struct Converager{
    std::array<V,N> arr_con = std::array<V,N>();
    size_t ctr=0;
    bool full=false;
    std::function<bool(V,V)> comparator= nullptr;


    void set_comparator(std::function<bool(V,V)> fun){comparator=fun;}

    void inset_elm(V&& v)
    {
        arr_con[++ctr%N]=std::forward<V>(v);
    }
    [[nodiscard]] bool is_converage()const
    {
        if(ctr<N) return false;
        assert(this->comparator!= nullptr);
        auto size_ctr = N-1;
        while(--size_ctr>0 && comparator(arr_con[size_ctr],arr_con[0]) );
        return size_ctr==0;
    }
    V acc(std::function<V(V&,V&)> acc)
    {
        std::accumulate(arr_con.begin(),arr_con.end(),0.0,acc);
    }
    size_t size(){return N;}


};
class SimulationGame{

    //Grid _g;
    short stop=0;
    std::chrono::duration<long,std::ratio<1,1>>::rep time_start;
    u_int32_t NUMBER=1000;
    u_int32_t iterationsMAX=1000000;//10000000;//50M//20 000 000/3000000;
    u_int64_t iterations=0;
    u_int ctr_action_defender=0;
    u_int32_t ctr=0;
    std::vector<u_int64_t > info = vector<u_int64_t>(4);
    std::unique_ptr<Agent> _attacker;
    std::unique_ptr<Agent> _defender;
    std::unique_ptr<State> _state;
    int last_mode=0;
    std::unique_ptr<Randomizer> random_object= nullptr;
    Grid *g= nullptr;
    double alpha=1.0;
    int stop_num =25;
    Saver<string> file_manger;
#ifdef TRAJECTORY
    Saver<string> trajectory_file;
#endif
    //Converager<15,std::vector<double>> converagerr;

public:

    SimulationGame(configGame &conf,std::unique_ptr<Agent> agentA,
                   std::unique_ptr<Agent> agentD,State *s,bool is_single=false):
            _attacker(std::move(agentA)),
            _defender(std::move(agentD)),
            _state(std::make_unique<State>(*s)),random_object(std::make_unique<Randomizer>(conf._seed))
            ,file_manger(conf.home+STR_HOME_DIR+std::to_string(conf._seed)+"_u"+conf.idNumber+"_L"+std::to_string(conf.eval_mode)+"_A"+std::to_string(conf.alpha)+"_Eval.csv",10)
            #ifdef TRAJECTORY
            ,trajectory_file(conf.home+STR_HOME_DIR+std::to_string(conf._seed)+"_u"+conf.idNumber+"_L"+std::to_string(conf.eval_mode)+"_Traj.csv",BUFFER_TRAJECTORY)//9000
            #endif
            {

        g=_state->g_grid;
        //this->iterationsMAX=std::max(g->getSizeIntGrid(),200000000);
        file_manger.set_header_vec({"episodes","Collision","Wall" ,"Goal" ,"Inconsistent","States","time_ms"});
        #ifdef TRAJECTORY
        init_trajectory_file(conf);
        ///treeTraversal();
        #endif
        time_start = conf.timeStart;
        this->_attacker->getPolicyInt()->prefix_file_name=std::to_string(conf._seed);
        this->_defender->getPolicyInt()->prefix_file_name=std::to_string(conf._seed);
        if(is_single)
        {
            stop_num=1;
            alpha=conf.alpha/10.0;
            if(conf.alpha > 10){
                this->alpha=1.0;
                this->iterationsMAX=1000*((conf.alpha)-10);
            }
        }

    }
    void init_trajectory_file(configGame &conf)
    {
        #ifdef TRAJECTORY
        //trajectory_file.set_header({"col"});
        trajectory_file.set_header_vec({"size"+conf.sizeGrid.to_str()});
        string str_goal="goal";
        std::for_each(conf.gGoals.begin(),conf.gGoals.end(),
                      [&](const Point &p){str_goal+=(p.to_str()+"_");});
        trajectory_file.set_header_vec({str_goal.substr(0, str_goal.size()-1)});
        #endif
    }
    void main_loop()
    {

        while(true)
        {
            reset();
            #ifdef PRINT
            cout<<"[S0] [real] "<<_state->to_string_state()<<endl;
            #endif
            while(true)
            {
                if(loop())
                    break;
            }

            #ifdef PRINT
            cout<<"END\n";
            #endif
            print_info();
            if(is_converage())
                break;
        }
        reset();
        cout<<"[Simulator] EDN"<<endl;
    }
    bool loop()
    {
        change_abstraction();
        //cout<<this->_state->to_string_state()<<"   last_mode: "<<last_mode<<endl;
        #ifdef PRINT
        cout<<"last_mode: "<<last_mode<<" [real] ";
        cout<<this->_state->to_string_state()<<" ";
        #endif
       // cout<<this->_state->to_string_state()<<endl;
        do_action_defender();
        attcker_do_action();

        chnage_time_step();
        return check_condtion();
    }
    void get_agents_data_policy()const
    {
        #ifdef A_DATA
        this->_attacker->getPolicy()->policy_data();
        #endif
        #ifdef Q_DATA
        this->_defender->getPolicy()->policy_data();
        #endif
    }
    std::unique_ptr<Agent>&& get_agnet_D(){return std::move(this->_defender);}
    void set_agnet_D(std::unique_ptr<Agent>&& D){this->_defender=std::move(D);}
private:

    void do_action_defender()
    {
        _defender->doAction(_state.get());
        #ifdef TRAJECTORY
        save_trajactory(_defender->get_id());
        #endif
        #ifdef PRINT
        cout<<"  [Action] "<<_defender->lastAction.to_str()<<"  \n";
        #endif
        _state->applyAction(_defender->get_name_id(),_defender->lastAction,_defender->get_max_speed(),last_mode);
        ctr_action_defender=last_mode;

    }
    void attcker_do_action()
    {
        _attacker->doAction_without_apply(_state.get(),last_mode);
        //cout<<"J: "<<last_mode<<endl;
        #ifdef TRAJECTORY
        save_trajactory(_attacker->get_id());
        #endif


    }
    inline void set_grid(){_state->g_grid=g;}
    bool check_condtion()
    {

        const Point& pos_A = this->_state->get_position_ref(this->_attacker->get_id());
        const Point& pos_D = this->_state->get_position_ref(this->_defender->get_id());
        //wall
        if(is_absolut_wall(pos_D))
        {
            #ifdef PRINT
            cout<<"[event] WallId => ";
            cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            #endif
            info[info::WallId]++;
            return true;
        }
        //goal
        if(is_absolut_goal(pos_A))
        {
            #ifdef PRINT
            cout<<"[event] GoalId => ";
            cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            #endif
            info[info::GoalId]++;
            return true;
        }
        //coll
        if(is_absolut_collision(pos_D,pos_A))
        {

            #ifdef PRINT
            cout<<"[event] CollId => ";
            cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            #endif
            info[info::CollId]++;
            return true;
        }

        return false;
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

    void set_mode_abstract(int step)
    {
        last_mode = step;
    }
    void reset()
    {

        #ifdef TRAJECTORY
        trajectory_file.save_string_body("END");
        #endif

        _defender->rest();
        _attacker.get()->rest();
        this->reset_state();
        //set_mode_abstract();
        ctr_action_defender=0;

    }
    bool is_converage()const
    {
        if(iterations>iterationsMAX ){
            cout<<"[iterationsMAX]"<<endl;
            return true;}
        if( stop>=stop_num){
            cout<<"[stop]"<<endl;
            return true;}
        return false;
    }
    void reset_state(){
        auto [pPos,sSpeed] = this->_defender->get_pos(this->random_object->get_double());
        setPosSpeed(sSpeed,pPos,this->_defender->get_id());

        auto posSpeed = this->_attacker->get_pos(this->random_object->get_double());
        setPosSpeed(posSpeed.second,posSpeed.first,this->_attacker->get_id());
        //change_abstraction();
        _state->takeOff=false;
        _state->set_budget(_defender->get_id(),1);
        _state->set_budget(_attacker->get_id(),0);

    }
    void setPosSpeed(const Point &sSpeed,const Point &pPos,State::agentEnum id_str)
    {
        _state->set_position(id_str,
                             pPos);
        _state->set_speed(id_str,sSpeed);
    }
    void print_info()
    {
        iterations++;
        if(!save_data())
            return;
        cout<<"iterations: "<<iterations<<"\t";
        cout<<"Coll: "<<this->info[info::CollId]<<"\t";
//        cout<<"Wall: "<<this->info[info::WallId]<<"\t";
//        cout<<"Goal: "<<this->info[info::GoalId]<<"\t";
        cout<<endl;
        clear_info();
    }
    bool save_data()
    {
        ctr++;
        if(ctr%NUMBER>0)
            return false;
        start_record_policy();
        u_int64_t ctr_states=0;
        if(info[info::CollId]>=NUMBER*alpha) {

            if(stop>1) {
                auto *ptr = dynamic_cast<RtdpAlgo *>(_defender->getPolicyInt());
                this->info[info::OpenId]=ptr->getUtilRTDP()->inconsistent;
                this->info[info::Size]=ptr->getUtilRTDP()->inconsistent;
                ctr_states = ptr->getUtilRTDP()->get_ctr_state();
            }
            stop += 1;
        }
        else{stop=0;}

        vector<double> x;
        x.reserve(info.size()+2);
        x.emplace_back(double(iterations)/double(iterationsMAX));
        for(auto item:info)
            x.emplace_back(item/double(NUMBER));
        x.emplace_back(ctr_states);
        //x.emplace_back(ctr_action_defender);
        const auto p1 = std::chrono::system_clock::now();
        x.emplace_back(std::chrono::duration_cast<std::chrono::microseconds>(
                (p1).time_since_epoch()).count()-time_start);



        file_manger.inset_data(x);
        file_manger.inset_endLine();

        x.erase(x.begin());
        //converagerr.inset_elm(std::move(x));

        return true;
    }
    void change_abstraction()
    {
        auto *ptr = dynamic_cast<RtdpAlgo*>(_defender->getPolicyInt());
        bool b = ptr->get_evaluator()->change_scope_(_state.get());
        int seq_action = ptr->get_evaluator()->get_Scheduler_ref().get_tmp();
        if(last_mode==seq_action)
            return;
        set_mode_abstract(seq_action);
    }
    void clear_info(){std::fill(info.begin(), info.end(), 0);}

    static bool comper_vectors(std::vector<double> x1,std::vector<double> x2)
    {
        for(int i=0;i<x1.size();++i)
            if(x1[i]!=x2[i]) return false;
        return true;
    }
    void chnage_time_step()
    {
        //cout<<_attacker->get_name_id()+last_mode<<endl;
        _state->set_budget(_attacker->get_name_id(),_state->get_budget(_attacker->get_name_id())+last_mode);
    }
    static void save_trajactory(State::agentEnum agent_name)
    {
        string name;
        if(agent_name==State::agentEnum::D)
             name = "D@";
        else  name = "A@";
#ifdef TRAJECTORY
        trajectory_file.save_string_body(name+_state->get_position_ref(agent_name).to_str());
#endif

    }

    void start_record_policy()
    {
        auto *ptr = dynamic_cast<RtdpAlgo *>(_defender->getPolicyInt());
        if(ptr->getUtilRTDP()->start_inset) return;
        if(stop>2 or iterations+20000>iterationsMAX)
            ptr->getUtilRTDP()->start_inset = true;
    }

    void treeTraversal()
    {
        auto *ptr = dynamic_cast<PathFinder*>(this->_attacker->getPolicyInt());
        auto myPaths = std::make_unique<vector<pair<double,vector<StatePoint>>>>();
        ptr->treeTraversal(_state.get(),myPaths.get());
        cout<<"[treeTraversal]"<<endl;
        for(const auto& item:*myPaths)
        {
            cout<<"P:"<<item.first<<";";
            for(const auto &step:item.second)
                cout<<step<<';';
            cout<<endl;
        }
        cout<<"all="<<myPaths->size()<<endl;

    }
};





#endif //TRACK_RACING_SIMULATOR_HPP
