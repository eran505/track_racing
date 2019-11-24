//
// Created by ise on 13.11.2019.
//

#ifndef RACING_CAR_GRID_HPP
#define RACING_CAR_GRID_HPP
#include <list>
#include <zconf.h>
#include <iostream>
#include <string>
#include "util_game.hpp"

using namespace std;
typedef unsigned int uint;
/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {

    uint X;
    uint Y;
    list<Point *>* list_goals;


};
/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/

class Grid{
private:
    //fields
    uint x_size;
    uint y_size;
    string **grid;
    list<Point *>* all_golas;

    void build_grid();

    public:
        Grid();
        void print_vaule();
        void init_grid(const game_params&);
        ~Grid();
        std::list<std::tuple<uint,uint>> get_goals();
};

class box{
public:
    ~box() = default;

    box() {
        this->info="";
    }

    string info;
    friend ostream& operator<<(ostream& os,  const box& my_box);
    void set_agent(string &str_agent);
};


#endif //RACING_CAR_GRID_HPP
