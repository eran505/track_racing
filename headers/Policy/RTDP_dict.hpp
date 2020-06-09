//
// Created by eranhe on 04/06/2020.
//

#ifndef TRACK_RACING_RTDP_DICT_HPP
#define TRACK_RACING_RTDP_DICT_HPP
#include "RTDP_util.hpp"

#define SizeActions

class RTDP_dict : RTDP_util{

    std::unique_ptr<std::unordered_map<u_int64_t , std::array<double,27> >>  qMapTable =
            std::make_unique<std::unordered_map<u_int64_t , std::array<double,27> >>();

public:
    RTDP_dict(int grid_size,vector<pair<int,int>>& max_speed_and_budget,string &mHome):
    RTDP_util(grid_size,max_speed_and_budget,mHome)
    {}
};



#endif //TRACK_RACING_RTDP_DICT_HPP
