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
    vector<pair<short,Point>> all_golas_data;




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
        bool isGoalReward(const Point& locPoint)
        {
            for(const auto& item : all_golas_data)
            {
                if (item.second==locPoint)
                    if(item.first>0)
                        return true;
            }
            return false;
        }
        bool is_goal_reward(const Point& loc){

            auto pos = find_if(all_golas_data.begin(), all_golas_data.end(), [&]( pair<short,Point>& s) {
                return s.second==loc;
            });
            if (pos==all_golas_data.end())
                return false;
            if (pos->first>0)
                return true;
            return false;
        }
        short get_goal_reward(const Point& loc)
        {
            auto pos = std::find_if(all_golas_data.begin(),all_golas_data.end(),[&](pair<short,Point>& itm){
                return loc==itm.second;
            });
            if (all_golas_data.end()==pos)
                return -1;
            return pos->first;
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
                    all_golas_data.emplace_back(vecB[i]?1:0,all_golas.operator[](i));
            }
        }
};



#endif //RACING_CAR_GRID_HPP
