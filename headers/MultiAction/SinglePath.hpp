//
// Created by eranhe on 09/08/2020.
//

#ifndef TRACK_RACING_SINGLEPATH_HPP
#define TRACK_RACING_SINGLEPATH_HPP
#include "Policy/Attacker/PathFinder.hpp"
#include "Simulator.hpp"

typedef vector<pair<double,vector<StatePoint>>> vector_p_path;
typedef std::unique_ptr<Agent> unique_agnet;
class SinglePath{

    std::unique_ptr<Agent> _attacker;
    std::shared_ptr<Agent> _defender;
    std::unique_ptr<State> _start_state;
    std::unique_ptr<vector_p_path> all_paths = nullptr;
    std::unique_ptr<Agent> _parital_attacker = nullptr;
    configGame config;
public:
    SinglePath(configGame &conf, State *s,unique_agnet A,unique_agnet D):
            _attacker(std::move(A)),
            _defender(std::move(D)),
            _start_state(std::make_unique<State>(*s)),
            config(conf)
    {
        get_all_paths();
        cout<<"Path Number: "<<all_paths->size()<<endl;
        //train_on_all_path();
        //exit(0);
    }

    void main_functopn_genrator()
    {
        std::for_each(all_paths->begin(),all_paths->end(),
                      [&](const pair<double,vector<StatePoint>>& item){
            train_single_path(item.first,item.second);
        });

        eval_all_paths();
    }
    void train_on_all_path()
    {
        cout<<"[eval policy]"<<endl;
        //_defender->evalPolicy();
        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                                _defender,_start_state.get());
        sim.main_loop();
        //_defender->trainPolicy();
    }
private:
    void train_single_path(const double path_probability, const std::vector<StatePoint>& path_states)
    {

        Policy *ptr = new PathFinder(_attacker->get_max_speed()
                ,_attacker->get_id(),config.home,path_probability,path_states);
        auto naive_attacker=std::make_unique<Agent>(_attacker->getAllPositions_copy(),adversary,1);
        naive_attacker->setPolicy(ptr);
        SimulationGame sim = SimulationGame(config,std::move(naive_attacker),
                                            _defender,_start_state.get());
        sim.main_loop();
    }

    void eval_all_paths()
    {
        cout<<"[eval policy]"<<endl;
        _defender->evalPolicy();
        SimulationGame sim = SimulationGame(config,std::move(_attacker),
                                            _defender,_start_state.get());
        sim.main_loop();
        _defender->trainPolicy();
    }
    void get_all_paths()
    {
        all_paths = std::make_unique<vector<pair<double,vector<StatePoint>>>>();
        const PathFinder *ptr = get_policy_attcker();
        ptr->treeTraversal(_start_state.get(),all_paths.get());

    }
    auto get_policy_attcker() -> const PathFinder*
    {
        return dynamic_cast<const PathFinder*>(_attacker->getPolicy());
    }
    auto get_policy_defender() -> const RtdpAlgo*
    {
        return dynamic_cast<const RtdpAlgo*>(_defender->getPolicy());
    }



};


#endif //TRACK_RACING_SINGLEPATH_HPP
