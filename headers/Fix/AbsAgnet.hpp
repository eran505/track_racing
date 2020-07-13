//
// Created by eranhe on 09/07/2020.
//

#ifndef TRACK_RACING_ABSAGNET_HPP
#define TRACK_RACING_ABSAGNET_HPP
#include "Agent.hpp"
#include "fixManager.hpp"

/**
 * Class responsibility: To switch between the right level of abstraction
 **/
class agentAbs{

    std::shared_ptr<Agent> _agnet;

public:

    [[nodiscard]] string get_id()const{ return _agnet->get_id();}

    void make_move(State *s) {
        //transform into the right state abstract
        //auto trans_state = transform_state(s);

        // do action
        //_agnet->doAction(trans_state.get());

        //apply action
        apply_action_actual_state(s);

    }

    void apply_action_actual_state(State *s)
    {
        s->applyAction(_agnet->get_id(), _agnet->lastAction,
                       _agnet->getPolicyInt()->max_speed);
    }
    void switch_abstraction(const State *s)
    {

    }

};
#endif //TRACK_RACING_ABSAGNET_HPP
