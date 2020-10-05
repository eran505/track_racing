//
// Created by ERANHER on 23.7.2020.
//

#ifndef TRACK_RACING_VANILLA_HPP
#define TRACK_RACING_VANILLA_HPP

#include <functional>
#include "Policy.hpp"
#include "State.hpp"
class Vanilla{

private:

    std::vector<Policy*> other_policies;
    double _stochasticMovement=1.0;
    string _agent_id;
    Policy* my_policy{};
    Point slide_action=Point(0);
    vector<pair<State*,double>> state_tran_q;
    unordered_map<int,Point> hashActionMap;
    std::function <void(const State *s,const Point &a)> stochastic_expander;
public:
    void clean()
    {
        for(auto &item:state_tran_q)
        {
            //cout<<item.first->to_string_state()<<endl;
            delete (item.first);
        }
        state_tran_q.clear();
    }
    vector<pair<State*,double>>& expnad_state(const State *s,const Point &a)
    {
        stochastic_expander(s,a);
        expand_other();
        return state_tran_q;
    }
    void set_stochasticMovement(double m)
    {
        _stochasticMovement=m;
        if(_stochasticMovement>=1)
            stochastic_expander=[&](const State *s,const Point &a)
            {expand_stochasticMovement_zero(s,a);};
        else
            stochastic_expander=[&](const State *s,const Point &a)
            {expand_stochasticMovement(s,a);};
    }

    Vanilla(double stochasticMovement,vector<Policy*>& tran,Policy *my_poly)
    {
        set_stochasticMovement(stochasticMovement);
        this->other_policies=tran;
        this->my_policy=my_poly;
        action_map();
    }

private:
    void expand_stochasticMovement_zero(const State *s,const Point &a) {
        state_tran_q.emplace_back(my_policy->apply_action_state(new State(*s), a),_stochasticMovement);
    }
    void expand_stochasticMovement(const State *s,const Point &a) {
        state_tran_q.emplace_back(my_policy->apply_action_state(new State(*s),slide_action),1-_stochasticMovement);
        state_tran_q.emplace_back(my_policy->apply_action_state(new State(*s),a),_stochasticMovement);
    }


    //TODO: why do i get error here?
    void expand_other()
    {
        for (Policy *item_policy: this->other_policies)
        {
            vector <pair<State*,double>> state_tran_q_tmp;
            for(const auto &pair_state_prob:this->state_tran_q)
            {
                auto options_actions=item_policy->TransitionAction(pair_state_prob.first);
                for(auto i = 0; i < options_actions->size(); i++)
                {
                    auto new_state =item_policy->apply_action_state(
                            new State(*pair_state_prob.first),
                            id_to_action(options_actions->operator[](i)));

                    state_tran_q_tmp.emplace_back(
                            new_state,
                            options_actions->operator[](++i)*pair_state_prob.second);
                }
                delete pair_state_prob.first;
            }
            state_tran_q=state_tran_q_tmp;
        }
    }
    const Point& id_to_action(int action_ID)
    {
        return this->hashActionMap.at(action_ID);
    }
    void action_map()
    {
        auto map = Point::getDictAction();

        for(auto &item:*map)
        {
            this->hashActionMap.try_emplace(item.first,*item.second);
            delete item.second;
        }
        delete map;
    }

};



#endif //TRACK_RACING_VANILLA_HPP
