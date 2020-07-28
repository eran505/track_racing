//
// Created by eranhe on 27/07/2020.
//

#ifndef TRACK_RACING_SCHEDULER_HPP
#define TRACK_RACING_SCHEDULER_HPP
#include <functional>
#include <utility>
#include "Policy.hpp"
#include "State.hpp"
#include "util_game.hpp"
#include "Agent.hpp"
#include "Fix/containerFix.hpp"
#include "MultiAction/MultiActionAgent.hpp"
class Scheduler{

    string attacker_id;
    string defender_id;
    std::shared_ptr<std::vector<containerFix>> _levels;
    u_int32_t idx_level=0;

public:
    Scheduler(string d,string a,int num_lev=2):
    attacker_id(std::move(a))
    ,defender_id(std::move(d))
    ,_levels(std::make_shared<std::vector<containerFix>>(num_lev))
    {
        for(int i=0;i<num_lev;++i)
        {
            auto &ref_item = _levels->emplace_back();
            ref_item.upper=Point(int(pow(2,i+3)),int(pow(2,i+3)),1);
        }

    }
    void change_action_abstraction(const State *s)
    {
        Point dif = get_dif(s);
        if(dif<_levels->operator[](idx_level+1).upper)
            idx_level++;
    }

private:
    Point get_dif(const State *s)
    {
        return (s->get_position_ref(attacker_id)-s->get_position_ref(defender_id)).AbsPoint();
    }

};


#endif //TRACK_RACING_SCHEDULER_HPP
