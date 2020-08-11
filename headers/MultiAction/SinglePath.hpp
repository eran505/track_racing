//
// Created by eranhe on 09/08/2020.
//

#ifndef TRACK_RACING_SINGLEPATH_HPP
#define TRACK_RACING_SINGLEPATH_HPP
#include "Policy/Attacker/PathFinder.hpp"
#include "Simulator.hpp"

typedef vector<pair<double,vector<StatePoint>>> vector_p_path;

class SinglePath{

    std::unique_ptr<Agent> _attacker;
    std::unique_ptr<Agent> _defender;
    std::unique_ptr<State> _start_state;
    std::unique_ptr<vector_p_path> all_paths = nullptr;

    configGame config;

    SinglePath(configGame &conf, State *s,Policy *policyA,Policy *policyD,
               std::vector<weightedPosition>& listPointA,
               std::vector<weightedPosition>& listPointD):
            _attacker(std::make_unique<Agent>(listPointA,adversary,1)),
            _defender(std::make_unique<Agent>(listPointD,gurd,1)),
            _start_state(std::make_unique<State>(*s)),
            config(conf)
    {}

    void main_functopn_genrator()
    {
        get_all_paths();
    }
    void train_single_path(const double path_probability, const std::vector<StatePoint>& path_states)
    {

        Policy *ptr = new PathFinder(_attacker->get_max_speed()
                ,_attacker->get_id(),config.home,path_probability,path_states);
        auto naive_attacker=std::make_unique<Agent>(_attacker->getAllPositions_copy(),adversary,1);
        naive_attacker->setPolicy(ptr);


    }


    void get_all_paths()
    {
        all_paths = std::make_unique<vector<pair<double,vector<StatePoint>>>>();
        const PathFinder *ptr = get_policy_attcker();
        ptr->treeTraversal(_start_state.get(),all_paths.get());

    }
    const PathFinder* get_policy_attcker()
    {
        return dynamic_cast<const PathFinder*>(_attacker->getPolicy());
    }



};


#endif //TRACK_RACING_SINGLEPATH_HPP
