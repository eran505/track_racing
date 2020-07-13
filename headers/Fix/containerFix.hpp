//
// Created by eranhe on 12/07/2020.
//

#ifndef TRACK_RACING_CONTAINERFIX_HPP
#define TRACK_RACING_CONTAINERFIX_HPP

#include "util_game.hpp"

//template<typename Val=double,size_t fix_size=27>
class containerFix{
    typedef std::vector<double> arr;
public:
    std::unique_ptr<unordered_map<u_int64_t,arr>> Q_table=std::make_unique<unordered_map<u_int64_t,arr>>();

    containerFix(){cout<<"build containerFix"<<endl;}
    containerFix(const containerFix& other)
    {
        for(const auto& item: *Q_table)
        {
            this->Q_table->insert(item);
        }
    }
    containerFix(containerFix&& other) noexcept
    {
        this->Q_table=std::move(other.Q_table);
    }
};

#endif //TRACK_RACING_CONTAINERFIX_HPP
