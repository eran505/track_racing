//
// Created by ERANHER on 23.7.2020.
//

#ifndef TRACK_RACING_VANILLA_HPP
#define TRACK_RACING_VANILLA_HPP

#include "Policy.hpp"
#include "State.hpp"
class Vanilla{

private:
    std::vector<Policy*> other_policies;
    double _stochasticMovement=1.0;
    bool is_stochasticMovement=false;
    string _agent_id;
    Policy* my_policy;
    int max_speed;
    const Point slide_action=Point(0);
    vector<pair<State*,double>> state_tran_q;
    std::unique_ptr<unordered_map<int,Point>> hashActionMap;
    static inline double reward(const State *s,const Point &a){return 0;}
    std::function <void(const State *s,const Point &a)> stochastic_expander;


    void expand_stochasticMovement_zero(const State *s,const Point &a) {
        state_tran_q.emplace_back(my_policy->apply_action_state(new State(*s), a),_stochasticMovement);
    }
    void expand_stochasticMovement(const State *s,const Point &a) {
        state_tran_q.emplace_back(my_policy->apply_action_state(new State(*s),slide_action),1-_stochasticMovement);
        state_tran_q.emplace_back(my_policy->apply_action_state(new State(*s),a),_stochasticMovement);
    }

    double expnad_state(const State *s,const Point &a)
    {
        stochastic_expander(s,a);
        expand_other();


        return 0.0;
    }

    void expand_other()
    {
        for (Policy *item_policy: this->other_policies)
        {
            vector <pair<State*,double>> state_tran_q_tmp;
            for(const auto &pair_state_prob:this->state_tran_q)
            {
                auto options_actions=item_policy->TransitionAction(pair_state_prob.first);
                for(auto i = 0; i != options_actions->size(); i++)
                {
                    auto new_state =item_policy->apply_action_state(
                            new State(*pair_state_prob.first),
                            id_to_action(options_actions->operator[](i))
                    );

                    state_tran_q_tmp.emplace_back(new_state,
                                                  options_actions->operator[](++i)*pair_state_prob.second);

                }
                delete pair_state_prob.first;
            }
            state_tran_q=state_tran_q_tmp;
        }
    }
    const Point& id_to_action(int action_ID)
    {
        return this->hashActionMap->at(action_ID);
    }

};



#endif //TRACK_RACING_VANILLA_HPP
