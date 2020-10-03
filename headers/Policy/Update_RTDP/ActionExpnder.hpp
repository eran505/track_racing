//
// Created by eranhe on 27/07/2020.
//

#ifndef TRACK_RACING_ACTIONEXPNDER_HPP
#define TRACK_RACING_ACTIONEXPNDER_HPP
#include <functional>
#include "Policy.hpp"
#include "State.hpp"
#include "MultiAction/Scheduler.hpp"

struct tuple_state{
    State state;
    double probability=0;
    bool is_end_state=true;
    tuple_state(State &&s,double p,bool end)
    :state(s),probability(p),is_end_state(end){}
};

class ActionExpnder{

    u_int16_t _action_seq=1;
    std::vector<Policy*> other_policies;
    double _stochasticMovement=1.0;
    string _agent_id;
    Policy* my_policy{};
    Point slide_action=Point(0);
    vector<tuple_state> stack;
    unordered_map<int,Point> hashActionMap;
    std::function <void(tuple_state &tuple_s,const Point &a)> stochastic_expander;
    vector<pair<State,double>> list_state_expnaded;


public:
    ActionExpnder(double stochasticMovement,vector<Policy*>& tran,Policy *my_poly):
    my_policy(my_poly)
    {
        set_stochasticMovement(stochasticMovement);
        this->other_policies=tran;
        action_map();
    }
    void clean()
    {
        for(auto &item:list_state_expnaded)
        {
            //delete (item.first);
        }
        list_state_expnaded.clear();
        stack.clear();
    }
    void set_seq_action(int num){_action_seq=num;}
    vector<pair<State,double>>& expnad_state(const State *s,const Point &a)
    {

        set_seq_action_by_state(s);
        stack.emplace_back(State(*s), 1.0, false);
        for(short i=_action_seq;i>0;--i)
        {
            // defender apply action
            std::for_each(stack.begin(),stack.end(),[&](tuple_state &item){stochastic_expander(item,a);});
            //print_stack(stack); for DEBUG
            expand_other(); // attacker apply action
            if_end_states(); // remove ended states from the list
        }
        transform();
        return list_state_expnaded;
    }
    void set_stochasticMovement(double m)
    {
        _stochasticMovement=m;
        if(_stochasticMovement>=1)
            stochastic_expander=[&](tuple_state &tuple_s,const Point &a)
            {expand_Movement_inplace(tuple_s,a);};
        else
            stochastic_expander=[&](tuple_state &tuple_s,const Point &a)
            {expand_stochasticMovement_inplace(tuple_s,a);};
    }
private:
    void expand_Movement_inplace(tuple_state &tuple_s,const Point &a,double p=1) {
        my_policy->apply_action_state(&tuple_s.state, a);
        tuple_s.probability*=p;
    }
    void expand_stochasticMovement_inplace(tuple_state &tuple_s,const Point &a) {
        auto new_tuple = tuple_state(State(tuple_s.state),tuple_s.probability,tuple_s.is_end_state);
        expand_Movement_inplace(tuple_s,a,_stochasticMovement);

        expand_Movement_inplace(new_tuple,slide_action,1.0-_stochasticMovement);
        stack.push_back(new_tuple);
    }

    void if_end_states()
    {
        for(auto &item:stack) item.is_end_state=condtion_game(&item.state);
    }
    void set_seq_action_by_state(const State *s)
    {
        auto seq = s->get_budget(this->my_policy->id_agent);
        this->_action_seq=seq;
    }
    //TODO: why do i get error here?
    void expand_other()
    {
        for (Policy *item_policy: this->other_policies)
        {
            vector<tuple_state> tmp;
            for(auto &tuple_item_state:this->stack)
            {
                if(tuple_item_state.is_end_state)
                {
                    tmp.push_back(tuple_item_state);
                    continue;
                }

                auto options_actions=item_policy->TransitionAction(&tuple_item_state.state);
                for(auto i = 0; i < options_actions->size(); i++)
                {
                    auto nState = State(tuple_item_state.state);
                    item_policy->apply_action_state(
                            nState,
                            id_to_action(options_actions->operator[](i)));

                    tmp.emplace_back(std::move(nState),tuple_item_state.probability*options_actions->operator[](++i),tuple_item_state.is_end_state);
                }
                //delete tuple_item_state.state;
            }
            stack.clear();
            stack=std::move(tmp);
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
    bool condtion_game(const State *s)
    {
        if(s->isGoal(my_policy->cashID)>=0)
            return true;
        if(s->g_grid->is_wall(s->get_position_ref(my_policy->id_agent)))
            return true;
        if(s->is_collusion(my_policy->id_agent,my_policy->cashID))
            return true;
        return false;
    }
    void transform()
    {
        for(auto &item:stack)
        {
            list_state_expnaded.emplace_back(std::move(item.state),item.probability);
        }
    }
    static void print_stack(vector<tuple_state>& vec)
    {
        cout<<"----------"<<endl;
        for(auto &item:vec) cout<<"[stack] { "<<item.state.to_string_state()<<", "<<item.probability<<" }"<<endl;
    }
};


#endif //TRACK_RACING_ACTIONEXPNDER_HPP
