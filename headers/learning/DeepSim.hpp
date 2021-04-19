////
//// Created by eranhe on 1/10/21.
////
//
//#ifndef TRACK_RACING_DEEPSIM_HPP
//#define TRACK_RACING_DEEPSIM_HPP
//#define OUTA
//#include <MultiAction/Simulator.hpp>
//#include "Policy.hpp"
//#include "Agent.hpp"
//#include "DeepAgent.hpp"
//#include "util/Rand.hpp"
//#include "util/saver.hpp"
//#include "util/utilClass.hpp"
//
//namespace DeepSim {
//
//    enum entryID : short{
//            CollId=0,WallId=1,GoalId=2
//    };
//
//    #define OUT_REC
//    #define STR_HOME_DIR "/car_model/out/"
//    #define BUFFER_SAVER 5
//
//    class DeepSim {
//        Rewards rewards=Rewards::getRewards();
//        u_int32_t game_counter=0;
//        u_int32_t last_mode=1;
//        std::unique_ptr<Agent> _attacker;
//        std::unique_ptr<deepAgent> _defender;
//        std::unique_ptr<State> _state;
//        std::unique_ptr<Randomizer> random_object = nullptr;
//        #ifdef OUT_REC
//        Saver<string> file_manger;
//        #endif
//        std::vector<u_int64_t > _info= vector<u_int64_t>(5);
//        static constexpr u_int32_t learning_itreation=200;
//        static constexpr u_int32_t print_every=100;
//        u_int32_t  steps=0;
//    public:
//        DeepSim(configGame &conf,std::unique_ptr<Agent> A,std::unique_ptr<deepAgent> D,State *s):
//        _attacker(std::move(A)),
//        _defender(std::move(D)),
//        _state(std::make_unique<State>(*s)),
//        random_object(std::make_unique<Randomizer>(conf._seed)),
//        #ifdef OUT_REC
//        file_manger(conf.home+STR_HOME_DIR+std::to_string(conf._seed)+"_u"+conf.idNumber+"_L"+std::to_string(conf.eval_mode)+"_A"+std::to_string(conf.alpha)+"_Eval.csv",BUFFER_SAVER)
//        #endif
//        {
//
//            // set the seed for the Attacker
//            this->_attacker->getPolicyInt()->prefix_file_name=std::to_string(conf._seed);
//
//            #ifdef OUT_REC
//            file_manger.set_header_vec({"episodes","Collision","Wall" ,"Goal" ,"time_ms"});
//            #endif
//        }
//
//
//        void main_loop()
//        {
//
//            while(true)
//            {
//                reset();
//
//                while(true)
//                {
//
//                    if(loop())
//                        break;
//                }
//
//                if(game_counter%print_every==0)
//                    eval();
//                print_info();
//                if(is_converage())
//                    break;
//            }
//            reset();
//            cout<<"[Simulator] EDN"<<endl;
//
//            #ifdef OUTA
//            this->_attacker->getPolicy()->policy_data();
//            #endif
//
//        }
//
//    private:
//
//        void eval()
//        {
//            u_int32_t ep_num=1;
//            int wall_s=_info[entryID::WallId];
//            int goal_s=_info[entryID::GoalId];
//            int coll_s=_info[entryID::CollId];
//
//            for (int i = 0; i < ep_num; ++i) {
//                reset();
//                while (true) {
//                    this->_defender->do_action_eval(_state.get());
//                    attcker_do_action();
//                    const auto[end, r]=check_condtion();
//                    if (end)
//                        break;
//                }
//            }
//            cout<<"\n-----EVAL----"<<endl;
//            cout<<"iterations:"<<steps<<"\t";
//            cout<<"Coll: "<<float(_info[entryID::CollId]-coll_s)/ep_num<<"\t";
//            cout<<"Goal: "<<float(_info[entryID::GoalId]-goal_s)/ep_num<<"\t";
//            cout<<"Wall: "<<float(_info[entryID::WallId]-wall_s)/ep_num<<"\t\t\t";
//            _info[entryID::WallId]=wall_s;
//            _info[entryID::GoalId]=goal_s;
//            _info[entryID::CollId]=coll_s;
//
//        }
//        bool loop()
//        {
//
//            steps++;
//            do_action_defender();
//            attcker_do_action();
//            const auto [end,r]=check_condtion();
//            _defender->learn(_state.get(),int(end),r);
//            return end;
//        }
//        void do_action_defender()
//        {
//            //_defender
//            this->_defender->do_action(_state.get());
//
//        }
//        inline void attcker_do_action(){
//            _attacker->doAction_without_apply(_state.get(),last_mode);
//        }
//        bool is_converage()
//        {
//            if (this->game_counter>this->learning_itreation)
//                return true;
//            return false;
//        }
//
//        void reset()
//        {
//            //_defender->rest();
//            _attacker.get()->rest();
//            this->reset_state();
//
//        }
//
//        void print_info()
//        {
//            game_counter++;
//            if(game_counter%print_every!=0)
//                return;
//            cout<<"iterations: "<<this->game_counter<<"\t";
//            cout<<"Coll: "<<this->_info[entryID::CollId]<<"\t";
//            cout<<"Goal: "<<this->_info[entryID::GoalId]<<"\t";
//            cout<<"Wall: "<<this->_info[entryID::WallId]<<"\t";
//            cout<<endl;
//            std::fill(_info.begin(), _info.end(), 0);
//
//        }
//        void reset_state(){
//            const weightedPosition& loc_obhect = this->_defender->get_pos(this->random_object->get_double());
//
//            setPosSpeed(loc_obhect.speedPoint,loc_obhect.positionPoint,this->_defender->get_id());
//
//            auto const posSpeed = this->_attacker->get_pos(this->random_object->get_double());
//            setPosSpeed(posSpeed.second,posSpeed.first,this->_attacker->get_id());
//            //change_abstraction();
//            _state->takeOff=false;
//            _state->set_budget(_defender->get_id(),1);
//            _state->set_budget(_attacker->get_id(),0);
//
//        }
//        pair<bool,float> check_condtion()
//        {
//
//            const Point& pos_A = this->_state->get_position_ref(this->_attacker->get_id());
//            const Point& pos_D = this->_state->get_position_ref(this->_defender->get_id());
//            //wall
//            if(is_absolut_wall(pos_D))
//            {
//
//                _info[entryID::WallId]++;
//                return {true,rewards.WallReward};
//            }
//            //goal
//            if(is_absolut_goal(pos_A))
//            {
//                _info[entryID::GoalId]++;
//                return {true,rewards.GoalReward};
//            }
//            //coll
//            if(is_absolut_collision(pos_A,pos_D))
//            {
//                _info[entryID::CollId]++;
//                return {true,rewards.CollReward};
//            }
//
//            return {false,rewards.Step_reward};
//        }
//
//
//        inline static bool is_absolut_collision(const Point& pos_D,const Point& pos_A)
//        {
//            return pos_A==pos_D;
//        }
//        inline bool is_absolut_wall(const Point& pos_D)
//        {
//            return this->_state->g_grid->is_wall(pos_D);
//        }
//        inline bool is_absolut_goal(const Point& pos_A)
//        {
//            // res == 0 - means that the goal is with zero reward for D
//            // res>0 - means that the goal is -reward for D agent
//            return _state->g_grid->get_goal_reward(pos_A)>=0;
//        }
//
//        void setPosSpeed(const Point &sSpeed,const Point &pPos,State::agentEnum id_str)
//        {
//            _state->set_position(id_str,
//                                 pPos);
//            _state->set_speed(id_str,sSpeed);
//        }
//
//
//
//
//
//    };
//}
//#endif //TRACK_RACING_DEEPSIM_HPP
