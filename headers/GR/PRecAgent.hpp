//
// Created by eranhe on 5/25/21.
//

#ifndef TRACK_RACING_PRECAGENT_HPP
#define TRACK_RACING_PRECAGENT_HPP

#include "Policy.hpp"
#include "GoalRec.hpp"
class PRecAgent : public Policy{



    State::agentEnum attackerID = State::agentEnum::A;

    GoalRecognition GR;

public:
    PRecAgent(int maxSpeedAgent, short agentId, string &home1, int seed);

    Point get_action(State *s) override;
    ~PRecAgent() override;

    void reset_policy() override;
    void policy_data() const override;
    std::vector<double>* TransitionAction(const State*) const override;

    void intial_args(const std::vector<std::vector<Point>> &pathz,vector<double> &&path_probabilties);


};


void PRecAgent::intial_args(const vector<std::vector<Point>> &pathz, vector<double> &&path_probabilties) {
    this->GR.load_agent_paths(pathz,std::move(path_probabilties));

}

PRecAgent::PRecAgent(int maxSpeedAgent, short agentId, string &home, int seed
                     ):Policy(maxSpeedAgent,agentId,home,seed) , GR(seed) {

}

Point PRecAgent::get_action(State *s){

    GR.set_my_location(s->get_position(this->id_agent));
    auto action  = GR.do_action(s->get_position(this->attackerID),
                                s->get_speed_ref(this->id_agent));
    return action;
}


void PRecAgent::reset_policy() {
    this->GR.reset_ptr();

}

void PRecAgent::policy_data() const {
    printf("PRecAgent::No Data is available");
}


PRecAgent::~PRecAgent() {
    cout<<"del PRecAgent"<<endl;
}



std::vector<double> *PRecAgent::TransitionAction(const State *s)const {
    return new std::vector<double>();
}



#endif //TRACK_RACING_PRECAGENT_HPP
