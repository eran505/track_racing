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
#define TRAJECTORY
//#define PRINT
#define STR_HOME_DIR "/car_model/out/"
#include "util/Rand.hpp"
namespace info{
    enum info : short{
        CollId=0,WallId=1,GoalId=2,OpenId=3,Size=4,
    };


}

class SimulationGame{

    //Grid _g;
    bool stop=false;
    u_int32_t NUMBER=1000;
    u_int32_t iterationsMAX=200001;
    u_int64_t iterations=0;
    u_int ctr_action_defender=0;
    u_int32_t ctr=0;
    std::vector<u_int64_t > info = vector<u_int64_t>(4);
    std::unique_ptr<Agent> _attacker;
    std::shared_ptr<Agent> _defender;
    std::unique_ptr<State> _state;
    int last_mode=0;
    std::unique_ptr<Randomizer> random_object= nullptr;
    Grid *g= nullptr;
    Saver<string> file_manger;
    Saver<string> trajectory_file;
    int seq_max_action=0;
    Converager<10,std::vector<double>> converagerr;
public:
    SimulationGame(configGame &conf,Policy *policyA,Policy *policyD,std::vector<weightedPosition>& listPointAttacker
            ,std::vector<weightedPosition>& listPointDefender,State *s,int levels=3):
            _attacker(std::make_unique<Agent>(listPointAttacker,adversary,1)),
            _defender(std::make_unique<Agent>(listPointDefender,gurd,1)),
            _state(std::make_unique<State>(*s)),random_object(std::make_unique<Randomizer>(conf._seed))
            ,file_manger(conf.home+STR_HOME_DIR+std::to_string(conf._seed)+"_u"+conf.idNumber+"_L"+std::to_string(conf.levelz)+"_Eval.csv",10)
            ,trajectory_file(conf.home+STR_HOME_DIR+std::to_string(conf._seed)+"_u"+conf.idNumber+"_L"+std::to_string(conf.levelz)+"_Traj.csv",10000)
            ,seq_max_action(levels)
    {
        _attacker->setPolicy(policyA);
        _defender->setPolicy(policyD);
        g=_state->g_grid;
        file_manger.set_header({"episodes","Collision","Wall" ,"Goal" ,"PassBy","moves"});
        trajectory_file.set_header({""});
        converagerr.set_comparator(comper_vectors);
    }

    void main_loop()
    {

        while(true)
        {
            reset();
            #ifdef PRINT
            cout<<"[real] "<<_state->to_string_state()<<endl;
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
            if(is_converage() or stop)
                break;
        }
    }
    bool loop()
    {
        //cout<<this->_state->to_string_state()<<endl;
        change_abstraction();
        //cout<<this->_state->to_string_state()<<endl;
        do_action_defender();
        //cout<<this->_state->to_string_state()<<endl;
        bool is_end_game = attcker_do_action();

        return is_end_game;
    }

private:

    void do_action_defender()
    {
        _defender->doAction(_state.get());
        #ifdef TRAJECTORY
        save_trajactory(_defender->get_id());
        #endif
        int ctrLocal=1;
        while(true)
        {
            //cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            if(last_mode <= ctrLocal)
                break;
            _defender->getPolicy()->apply_action_state(_state.get(),_defender->lastAction);
            ctrLocal++;
            #ifdef TRAJECTORY
            save_trajactory(_defender->get_id());
            #endif
            ctr_action_defender++;
        }
        ctr_action_defender++;
    }
    bool attcker_do_action()
    {

        for(int i=0;i<last_mode;++i)
        {
            _attacker->doAction(_state.get());
            #ifdef TRAJECTORY
            save_trajactory(_attacker->get_id());
            #endif
            //cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
            if(check_condtion())
                return true;
        }
        return false;
    }
    inline void set_grid(){_state->g_grid=g;}
    bool check_condtion()
    {
        #ifdef PRINT
        cout<<"[real] "<<_state->to_string_state()<<"[MultiAction: "<<last_mode<<"]"<<endl;
        #endif
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
        //passBy
//        for(int i=0;i<pos_A.capacity;++i)
//            if((pos_A[i]>pos_D[i]))
//            {
//                info[info::OpenId]++;
//                return true;
//            }
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
        trajectory_file.inset_one_item_endLine("END");
        _attacker.get()->rest();
        _defender->rest();
        this->reset_state();
        //set_mode_abstract();
        ctr_action_defender=0;
    }
    bool is_converage()const
    {
        if(iterations>iterationsMAX)
            return true;
//        if(converagerr.is_converage())
//            return true;
        return false;
    }
    void reset_state(){
        auto [pPos,sSpeed] = this->_defender->get_pos(this->random_object->get_double());
        setPosSpeed(sSpeed,pPos,this->_defender->get_id());

        auto posSpeed = this->_attacker->get_pos(this->random_object->get_double());
        setPosSpeed(posSpeed.second,posSpeed.first,this->_attacker->get_id());
        //change_abstraction();

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
        if(!save_data())
            return;
        cout<<"iterations: "<<iterations<<"\t";
        cout<<"Coll: "<<this->info[info::CollId]<<"\t";
        cout<<"Wall: "<<this->info[info::WallId]<<"\t";
        cout<<"Goal: "<<this->info[info::GoalId]<<"\t";
        cout<<"PassBy: "<<this->info[info::OpenId]<<"\t";
        cout<<endl;
        clear_info();
    }
    bool save_data()
    {
        ctr++;
        if(ctr%NUMBER>0)
            return false;
        vector<double> x;
        x.emplace_back(double(iterations)/double(iterationsMAX));
        for(auto item:info)
            x.emplace_back(item/double(NUMBER));
        x.emplace_back(ctr_action_defender);
        if(info[info::CollId]==NUMBER)
            stop=true;
        file_manger.inset_data(x);
        file_manger.inset_endLine();
        x.erase(x.begin());
        converagerr.inset_elm(std::move(x));
        return true;
    }
    void change_abstraction()
    {
        RtdpAlgo *ptr = dynamic_cast<RtdpAlgo*>(_defender->getPolicyInt());
        bool b = ptr->get_evaluator()->change_scope_(_state.get());
        auto step = ptr->get_evaluator()->get_Scheduler().get_steps();
        if(last_mode==step)
            return;
        set_mode_abstract(step);
    }
    void clear_info(){std::for_each(info.begin(),info.end(),[](auto &item){item=0;});}

    static bool comper_vectors(std::vector<double> x1,std::vector<double> x2)
    {
        for(int i=0;i<x1.size();++i)
            if(x1[i]!=x2[i]) return false;
        return true;
    }
    void save_trajactory(const string &agent_name)
    {
        trajectory_file.inset_one_item_endLine(agent_name+"@"+_state->get_position_ref(agent_name).to_str());
    }
};





#endif //TRACK_RACING_SIMULATOR_HPP
