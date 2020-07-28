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
#define DEBUGING
#define STR_HOME_DIR "/car_model/out/"
struct Randomizer{
public:
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    explicit Randomizer(int seed):
            generator(seed),
            distribution(0.0,1.0)
    {

    }
    double get_double(){return this->distribution(this->generator);}
};

namespace info{
    enum info : short{
        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4,
    };


}

class SimulationGame{

    //Grid _g;
    u_int64_t iterations=0;
    u_int32_t ctr=0;
    std::vector<u_int64_t > info = vector<u_int64_t>(4);
    std::unique_ptr<Agent> _attacker;
    std::shared_ptr<Agent> _defender;
    std::unique_ptr<State> _state;
    int last_mode=-1;
    std::unique_ptr<Randomizer> random_object= nullptr;
    Grid *g= nullptr;
    Saver<string> file_manger;
    int seq_max_action=0;
public:
    SimulationGame(configGame &conf,Policy *policyA,Policy *policyD,std::vector<weightedPosition>& listPointAttacker
            ,std::vector<weightedPosition>& listPointDefender,State *s,int levels=3):
            _attacker(std::make_unique<Agent>(listPointAttacker,adversary,1)),
            _defender(std::make_unique<Agent>(listPointDefender,gurd,1)),
            _state(std::make_unique<State>(*s)),random_object(std::make_unique<Randomizer>(conf._seed))
            ,file_manger(conf.home+STR_HOME_DIR+std::to_string(conf._seed)+".csv",10)
            ,seq_max_action(levels)
    {
        _attacker->setPolicy(policyA);
        _defender->setPolicy(policyD);
        g=_state->g_grid;
        file_manger.set_header({"Collision","Wall" ,"Goal" ,"PassBy"
                                       ,"Down0","Down1","Down2","key0","key1","key2"});

    }
    void main_loop()
    {

        while(true)
        {
            reset();

            cout<<"[real] "<<_state->to_string_state()<<endl;
            while(true)
            {
                if(loop())
                    break;
            }
            if(is_converage())
                break;
            cout<<"END\n";
            print_info();
        }
    }
    bool loop()
    {
        change_abstraction();
        cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;

        do_action_defender();

        bool is_end_game = attcker_do_action();

        return is_end_game;
    }

private:

    void do_action_defender()
    {
        _defender->doAction(_state.get());
        int ctrLocal=0;
        while(true)
        {
            //cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            if(last_mode <= ctrLocal)
                break;
            _defender->getPolicy()->apply_action_state(_state.get(),_defender->lastAction);
            ctrLocal++;
        }
    }
    bool attcker_do_action()
    {
        for(int i=0;i<=last_mode;++i)
        {
            _attacker->doAction(_state.get());
            //cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            if(check_condtion())
                return true;
        }
        return false;
    }
    inline void set_grid(){_state->g_grid=g;}
    bool check_condtion()
    {
        const Point& pos_A = this->_state->get_position_ref(this->_attacker->get_id());
        const Point& pos_D = this->_state->get_position_ref(this->_defender->get_id());

        //wall
        if(is_absolut_wall(pos_D))
        {
            info[info::WallId]++;
            return true;
        }
        //goal
        if(is_absolut_goal(pos_A))
        {
            info[info::GoalId]++;
            return true;
        }
        //coll
        if(is_absolut_collision(pos_D,pos_A))
        {
            info[info::CollId]++;
            return true;
        }

        for(int i=0;i<pos_A.capacity;++i)
            if((pos_A[i]>pos_D[i]))
            {
                info[info::OpenId]++;
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

    void set_mode_abstract()
    {
        last_mode =  _state->get_budget(_defender->get_id());
    }
    void reset()
    {

        _attacker.get()->rest();
        this->reset_state();
        _defender->rest();
        set_mode_abstract();
    }
    bool is_converage() const
    {
        if(iterations>50000000)
            return true;
        return false;
    }
    void reset_state(){
        auto [pPos,sSpeed] = this->_defender->get_pos(this->random_object->get_double());
        setPosSpeed(sSpeed,pPos,this->_defender->get_id());

        auto posSpeed = this->_attacker->get_pos(this->random_object->get_double());
        setPosSpeed(posSpeed.second,posSpeed.first,this->_attacker->get_id());
        _state->set_budget(this->_defender->get_id(),seq_max_action-1);
        //assert(_state->get_budget(_defender->get_id())<2);
    }
    void setPosSpeed(const Point &sSpeed,const Point &pPos,const string &id_str)
    {
        _state->set_position(id_str,
                             pPos);
        _state->set_speed(id_str,sSpeed);
    }
    void print_info()
    {
        iterations++;
        cout<<"Coll: "<<this->info[info::CollId]<<"\t";
        cout<<"Wall: "<<this->info[info::WallId]<<"\t";
        cout<<"Goal: "<<this->info[info::GoalId]<<"\t";
        cout<<"PassBy: "<<this->info[info::OpenId]<<"\t";
        cout<<endl;
        save_data();

    }
    void save_data()
    {
        ctr++;
        if(ctr%1000>0)
            return;
        vector<u_int32_t> x;
        for(auto item:info)
            x.emplace_back(item);
        file_manger.inset_data(x);
        file_manger.inset_endLine();
    }
    void change_abstraction()
    {
        RtdpAlgo *ptr = dynamic_cast<RtdpAlgo*>(_defender->getPolicyInt());
        bool b = ptr->get_evaluator()->change_scope_(_state.get());
        if(b)
            set_mode_abstract();
    }
};





#endif //TRACK_RACING_SIMULATOR_HPP
