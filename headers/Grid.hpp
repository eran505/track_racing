//
// Created by ise on 13.11.2019.
//

#ifndef RACING_CAR_GRID_HPP
#define RACING_CAR_GRID_HPP
#include <list>
//#include <zconf.h>
#include <iostream>
#include <string>
#include "util_game.hpp"

using namespace std;
typedef unsigned int uint;
/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {

    Point size;
    list<Point *>* list_goals;


};
/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/

class Grid{
private:
    //fields
    Point size_point;
    string **grid;
    list<Point *>* all_golas;



    public:
        Point getPointSzie(){ return size_point;}
        Grid(const game_params&);
        void print_vaule();
        ~Grid();
        list<Point*> get_goals() {
            return *all_golas;
        }
        int getSizeIntGrid(){
            int size = 1;
            for (int i = 0; i < this->size_point.capacity; ++i) {
                size*=this->size_point.array[i];
            }
            return size;
        }
        bool is_at_goal(const Point* loc_point ){
            for (auto item_goal : *(this->all_golas)){
                if (item_goal->is_equal(loc_point))
                    return true;
            }
            return false;
        }
        bool is_wall(Point *ptr_point_loc){
            return ptr_point_loc->out_of_bound(this->size_point);
        }

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
