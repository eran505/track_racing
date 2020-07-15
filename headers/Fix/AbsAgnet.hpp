//
// Created by eranhe on 09/07/2020.
//

#ifndef TRACK_RACING_ABSAGNET_HPP
#define TRACK_RACING_ABSAGNET_HPP
#include <utility>

#include "Agent.hpp"
#include "Fix/fixAbstractLevel.hpp"
/**
 * Class responsibility: To switch between the right level of abstraction
 **/
class agentAbs{

    std::shared_ptr<Agent> _agnet= nullptr;

public:
    explicit agentAbs(std::shared_ptr<Agent> &agent_D){
        _agnet = agent_D;
    }
    agentAbs()= default;
    [[nodiscard]] string get_id()const{ return _agnet->get_id();}
    bool is_inbound(const State *s)const{
        return s->g_grid->is_wall(s->get_position_ref(_agnet->get_id()));
    }
    Agent* get_agent(){return _agnet.get();}
    void make_move(State *s) {
        //transform into the right state abstract
        //auto trans_state = _transform_state(s);

        // do action
        _agnet->doAction(s);

        //apply action
        apply_action_actual_state(s);

    }

    void apply_action_actual_state(State *s)const
    {
        s->applyAction(_agnet->get_id(), _agnet->lastAction,
                       _agnet->getPolicyInt()->max_speed);
    }
    void switch_abstraction(const State *s)
    {

    }
    [[nodiscard]] Point get_last_action()const{return _agnet->lastAction;}
    [[nodiscard]] int get_max_speed()const{return this->_agnet->getPolicy()->max_speed;}

    void retrun_dict(fixAbstractLevel &obj)
    {
        obj.return_dict(std::move(get_RTDP_util()->get_q_table()));
    }
    void update_stack()
    {
        this->_agnet->rest();
    }
    void set_dict(fixAbstractLevel &obj)
    {
        get_RTDP_util()->set_q_table(std::move(obj.get_dict()));
    }
    void end(fixAbstractLevel &obj)
    {
        update_stack();
        retrun_dict(obj);
    }
    void change_abstrct_point(const Point& offset,const Point& abs)
    {
        get_RtdpAlgo()->abstract=true;
        get_RtdpAlgo()->abs=abs;
        get_RtdpAlgo()->offset=offset;


    }
private:
    RtdpAlgo* get_RtdpAlgo()
    {
        return dynamic_cast <RtdpAlgo*>(this->_agnet->getPolicyInt());

    }
    RTDP_util* get_RTDP_util()
    {
        return this->get_RtdpAlgo()->getUtilRTDP();
    }
};
#endif //TRACK_RACING_ABSAGNET_HPP
