//
// Created by ise on 17.11.2019.
//

#ifndef RACING_CAR_MDPPLANER_HPP
#define RACING_CAR_MDPPLANER_HPP

#include "Grid.hpp"
#include "Agent.hpp"
#include "util_game.hpp"
#include <map>
#include "State.hpp"
class MdpPlaner{




public:


    std::unique_ptr<State> make_inital_state(Agent *ptr1,Agent *ptr2,Grid *g)
    {
        auto s = std::make_unique<State>();
        auto positionSpeed = ptr1->get_pos(0);
        s->add_player_state(ptr1->get_name_id(),positionSpeed.first,positionSpeed.second,ptr1->get_budget());
        auto positionSpeed2 = ptr2->get_pos(0);
        s->add_player_state(ptr2->get_name_id(),positionSpeed2.first,positionSpeed2.second,ptr2->get_budget());
        s->g_grid=g;
        return s;

    }



};

#endif //RACING_CAR_MDPPLANER_HPP
