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
    u_int idx_level=-1;

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
            //ref_item.upper=Point(int(pow(2,i+3)),int(pow(2,i+3)),4);
            ref_item.upper=Point(int(pow(2,i+3)),int(pow(2,i+3)),4);
            ref_item.step=get_step_number(pow(2,i+3));
            //ref_item.step=i+1;
        }
        idx_level=_levels->size()-1;
    }
    [[nodiscard]] int get_steps()const{return _levels->at(idx_level).step;}
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
        //cout<<"dif:"<<dif.to_str()<<endl;
        int delta=0;
        int delta_acc;
        //cout<<"cur:\t"<<idx_level<<" dif: "<<dif.to_str()<<"  get_step_number: "<<get_step_number(std::max(dif[0],std::max(dif[1],dif[2])))<<endl;
//        int scope_id = get_step_number(std::max(dif[0],std::max(dif[1],dif[2])));
//        if(idx_level==scope_id)
//            return 0;
//        change_scoper(rtdp,scope_id);
//        return scope_id;

        while(true)
        {
            delta_acc=0;
            if(idx_level>0) {
                if (dif < _levels->operator[](idx_level - 1).upper) {
                    change_scoper(rtdp, -1);
                    delta_acc+=-1;
                }
            }
            if(idx_level<_levels->size()-1) {
                if (!(dif < _levels->operator[](idx_level).upper)) {
                    change_scoper(rtdp, 1);
                    delta_acc+=1;
                }
            }
            if(delta+delta_acc==delta)
                break;
            delta+=delta_acc;
        }
        return delta;
    }

    void change_scoper(RTDP_util *rtdp,int delta_chage)
    {
        return_Q_table(std::move(rtdp->get_q_table()));
        idx_level+=delta_chage;
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
    static int get_step_number(int diff_dist)
    {
        double logme= log2(diff_dist);
        auto x = std::floor(logme)-3;
        int res = std::pow(2,x);
        return res;
    }

};


#endif //TRACK_RACING_SCHEDULER_HPP
