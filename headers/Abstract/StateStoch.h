//
// Created by eranhe on 23/05/2020.
//

#ifndef TRACK_RACING_STATESTOCH_H
#define TRACK_RACING_STATESTOCH_H
#include "util_game.hpp"
#include "State.hpp"



class stochasticState : State{
    float stoSpeedD;

public:
    explicit stochasticState(float x){this->stoSpeedD=x;}
    vector<State*> get;
    [[nodiscard]] float getProb(const string &id) const
    {
        int indexI;
        if(id[id.size()==Section::adversary])
            return 0.0;
        else
            return stoSpeedD;
    }
    void setSpeedDefender(float x)
    {
        stoSpeedD=x;
    }
    bool applyAction(const string &id, const Point &action, int max_speed) override {
        auto pstayInPlace = getProb(id);
        return applyer(id, action, max_speed, pstayInPlace);

    }
    bool applyer(const string &id, const Point &action, int max_speed,float SpeedP)
    {
        auto pos = this->speed_dict.find(id);
        if (pos==this->speed_dict.end())
            throw;
        // sot
        if (SpeedP!=0)
        {
            if (SpeedP<1.0/range_random(0,100))
                pos->second+=Point(0);
            else
                pos->second+=action;
        }
        else{pos->second+=action;}
        pos->second.change_speed_max(max_speed);
        auto pos_on_grid = this->pos_dict.find(id);
        pos_on_grid->second+=pos->second;
        auto outBound = this->g_grid->is_wall(&(pos_on_grid->second));
        return outBound;
    }
};


#endif //TRACK_RACING_STATESTOCH_H
