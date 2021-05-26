//
// Created by eranhe on 5/25/21.
//

#ifndef TRACK_RACING_PRECAGENT_HPP
#define TRACK_RACING_PRECAGENT_HPP

#include "Policy.hpp"
class PRecAgent : Policy{
    ~PRecAgent() override= default;

    PRecAgent(int maxSpeedAgent, short agentId, string &home1,
              const string& namePolicy, int speed_MAX, int seed, const string& agentID, string &home);

    Point get_action(State *s) override;
    void make_action(State *s,int jumps)override;
    void reset_policy() override;
    void minimization()override;
    void update_final_state(State *s)override;
    void learnRest()override;
    void policy_data()override;
    bool isInPolicy(const State *s)const override;
    const vector<double>* TransitionAction(const State *s)override;

};
PRecAgent::PRecAgent() = default;

Point PRecAgent::get_action(State *s) {
    return Point();
}

void PRecAgent::make_action(State *s, int jumps) {
    Policy::make_action(s, jumps);
}

void PRecAgent::reset_policy() {
    Policy::reset_policy();
}

void PRecAgent::minimization() {
    Policy::minimization();
}

void PRecAgent::update_final_state(State *s) {
    Policy::update_final_state(s);
}

void PRecAgent::learnRest() {
    Policy::learnRest();
}

void PRecAgent::policy_data() {

}

bool PRecAgent::isInPolicy(const State *s) const {
    return Policy::isInPolicy(s);
}

const vector<double> *PRecAgent::TransitionAction(const State *s) {
    return nullptr;
}


PRecAgent::PRecAgent(int maxSpeedAgent, short agentId, string &home1, const string& namePolicy, int speed_MAX, int seed,
         const string& agentID, string &home) : Policy(maxSpeedAgent, agentId, home1, seed) {

}


#endif //TRACK_RACING_PRECAGENT_HPP
