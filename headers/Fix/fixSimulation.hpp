
//
// Created by eranhe on 09/07/2020.
//

#ifndef TRACK_RACING_FIXSIMULATION_HPP
#define TRACK_RACING_FIXSIMULATION_HPP
#include "util_game.hpp"
#include "Policy.hpp"
#include "Agent.hpp"
#include "fixManager.hpp"
#define DEBUGING
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

class fixSimulation{

    //Grid _g;
    std::vector<u_int64_t > info = vector<u_int64_t>(4);
    std::unique_ptr<Agent> _attacker;
    std::shared_ptr<Agent> _defender;
    std::unique_ptr<State> _state;
    fixManager _manager;
    std::unique_ptr<Randomizer> random_object= nullptr;
    Grid *g= nullptr;
public:
    fixSimulation(configGame &conf,Policy *policyA,Policy *policyD,std::vector<weightedPosition>& listPointAttacker
    ,std::vector<weightedPosition>& listPointDefender,std::vector<pair<Point,Point>> &levels,State *s):
    _attacker(std::make_unique<Agent>(listPointAttacker,adversary,1)),
    _defender(std::make_unique<Agent>(listPointDefender,gurd,1)),
    _state(std::make_unique<State>(*s)),random_object(std::make_unique<Randomizer>(conf._seed))
    {
        _attacker->setPolicy(policyA);
        _defender->setPolicy(policyD);
        g=_state->g_grid;
        _manager = fixManager(conf,std::move(levels),_defender,_state.get());

    }
    void main_loop()
    {
        cout<<"[real] "<<_state->to_string_state()<<endl;
        while(true)
        {
            reset();
            while(true)
            {
                if(loop())
                    break;
            }
            this->_manager.end();
            if(is_converage())
                break;
            cout<<"END\n";
            print_info();
        }
    }
    bool loop()
    {

        while(_manager.managing(_state.get()));
#ifdef DEBUGING
        //cout<<_state->to_string_state()<<endl;
#endif

        _manager.make_action(_state.get());

        _attacker->doAction(_state.get());

         return check_condtion();
    }
private:
    inline void set_grid(){_state->g_grid=g;}
    bool check_condtion()
    {
        cout<<"[real] "<<_state->to_string_state()<<endl;
        set_grid();
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


    void reset()
    {

        _manager.reset(_state.get());
        _attacker.get()->rest();
        this->reset_state();

    }
    static bool is_converage() {return false;}
    void reset_state(){
        auto [pPos,sSpeed] = this->_defender->get_pos(this->random_object->get_double());
        setPosSpeed(sSpeed,pPos,this->_defender->get_id());

        auto posSpeed = this->_attacker->get_pos(this->random_object->get_double());
        setPosSpeed(posSpeed.second,posSpeed.first,this->_attacker->get_id());
    }
    void setPosSpeed(const Point &sSpeed,const Point &pPos,const string &id_str)
    {
        _state->set_position(id_str,
                                   pPos);
        _state->set_speed(id_str,sSpeed);
    }
    void print_info()
    {
        _manager.get_ctr_insetrtion();
        cout<<"Coll: "<<this->info[info::CollId]<<"\t";
        cout<<"Wall: "<<this->info[info::WallId]<<"\t";
        cout<<"Goal: "<<this->info[info::GoalId]<<"\t";
        cout<<"PassBy: "<<this->info[info::OpenId]<<"\t";
        cout<<endl;
    }
};



#endif //TRACK_RACING_FIXSIMULATION_HPP
