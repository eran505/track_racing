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


typedef unsigned int uint;
/*--------------------------------------------------------------------------------
								  Auxiliary Structures
--------------------------------------------------------------------------------*/
struct game_params {

    Point size;
    vector<Point> list_goals;


};
/*--------------------------------------------------------------------------------
									Class Declaration
--------------------------------------------------------------------------------*/

class Grid{
private:
    //fields
    Point size_point;
    vector<Point> all_golas;
    vector<Point> goalTarget;
    unordered_map<unsigned long,short> dictIsTarget;



    public:
        Point getPointSzie(){ return size_point;}
        Grid(game_params&);
        void print_vaule();

        vector<Point> get_goals() {
            return all_golas;
        }
        int getSizeIntGrid(){
            int size = 1;
            for (int i = 0; i < this->size_point.capacity; ++i) {
                if (this->size_point.array[i]==0) continue;
                size*=this->size_point.array[i];
            }
            return size;
        }
        bool isGoalReward(const Point* locPoint)
        {
            for (const auto &itemGoal : this->goalTarget ){
                if (itemGoal.is_equal(locPoint))
                    return true;
            }
            return false;
        }
        short isEnd(const Point* loc_point ){
            auto idH = loc_point->expHash();
            auto pos = dictIsTarget.find(idH);
            if (pos== dictIsTarget.end())
                return -1;
            return pos.operator*().second;
        }
        bool is_at_goal(const Point* loc_point ){
            for (const auto &item_goal : this->all_golas){
                if (item_goal.is_equal(loc_point))
                    return true;
            }
            return false;
        }
        bool is_at_goal(const Point &loc){
            return (std::find(all_golas.begin(),all_golas.end(),loc) != all_golas.end());
        }
        bool is_wall(Point *ptr_point_loc){
            return ptr_point_loc->out_of_bound(this->size_point);
        }
        bool is_wall(const Point &loc)
        {
            return loc.out_of_bound(this->size_point);
        }
        void setTargetGoals(const vector<bool> &vecB){
            for (int i = 0; i < vecB.size(); ++i) {
                if (vecB[i])
                    goalTarget.push_back(all_golas.operator[](i));
                this->dictIsTarget.insert({all_golas.operator[](i).expHash(),vecB[i]?1:0});
            }
        }
};



#endif //RACING_CAR_GRID_HPP
