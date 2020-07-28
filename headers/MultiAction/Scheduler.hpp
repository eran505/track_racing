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
#include "Policy/RTDP_util.hpp"
#include <cassert>
class Scheduler{

    string attacker_id;
    string defender_id;
    std::shared_ptr<std::vector<containerFix>> _levels;
    u_int idx_level=0;

public:
    Scheduler(string a,string d,int num_lev):
    attacker_id(std::move(a))
    ,defender_id(std::move(d))
    ,_levels(std::make_shared<std::vector<containerFix>>(0))
    {
        for(int i=0;i<num_lev;++i)
        {
            auto &ref_item = _levels->emplace_back();
            //ref_item.upper=Point(int(pow(2,i+3)),int(pow(2,i+3)),1);
            ref_item.upper=Point(int(pow(2,i+3)),int(pow(2,i+3)),1);
        }
        idx_level=_levels->size()-1;
    }
    int get_idx(){return idx_level;}
    void change_static(int idx, RTDP_util *ptr)
    {
        if(idx_level==idx)
            return;
        return_Q_table(ptr->get_q_table());
        idx_level=idx;
        ptr->set_q_table(get_Q_table());

    }
    int change_action_abstraction(const State *s,RTDP_util *rtdp)
    {
        //idx_level=s->get_budget(defender_id);
        Point dif = get_dif(s);
        if(idx_level>0) {
            if (dif < _levels->operator[](idx_level - 1).upper) {
                change_scoper(rtdp, -1);
                return -1;
            }
        }
        if(dif>_levels->operator[](idx_level).upper)
        {
            change_scoper(rtdp,1);
            return 1;
        }
        return 0;
    }
    void change_scoper(RTDP_util *rtdp,int delta_chage)
    {
        return_Q_table(std::move(rtdp->get_q_table()));
        idx_level=idx_level+delta_chage;
        rtdp->set_q_table(get_Q_table());
    }
    qTbale_dict get_Q_table()
    {
        //cout<<"Q<-["<<idx_level<<"]"<<endl;
        return std::move(_levels->operator[](idx_level).q);
    }
    void return_Q_table(qTbale_dict table)
    {
        //cout<<"["<<idx_level<<"]<-Q"<<endl;
        _levels->operator[](idx_level).q = std::move(table);
    }
    void reset(RTDP_util *rtdp){
        return_Q_table(rtdp->get_q_table());
        idx_level=_levels->size()-1;
        assert(idx_level>=0 and idx_level<_levels->size());
        rtdp->set_q_table(get_Q_table());
    }
private:
    Point get_dif(const State *s)
    {
        return (s->get_position_ref(attacker_id)-s->get_position_ref(defender_id)).AbsPoint();
    }


};


#endif //TRACK_RACING_SCHEDULER_HPP
