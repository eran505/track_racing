//
// Created by ise on 13.11.2019.
//

#ifndef RACING_CAR_GRID_HPP
#define RACING_CAR_GRID_HPP
#include <list>
//#include <zconf.h>
#include <iostream>
#include <string>
#include <utility>
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
    Point upperBound;
    vector<Point> all_golas;
    vector<pair<float,Point>> all_golas_data;
    Point lowerBound=Point(0);



    public:
        Point getPointSzie(){ return upperBound;}
        explicit Grid(game_params&);
        ~Grid() {cout<<"grid del\n"; }
        Grid(const Point& sizeP,vector<Point> allPoint,vector<short> rewardGoal)
        {
            upperBound=sizeP;
            all_golas=std::move(allPoint);
            for(size_t i=0;i<rewardGoal.size();++i)
            {
                all_golas_data.emplace_back(rewardGoal[i],all_golas[i]);
            }
        }
        Grid(const Point& sizeP,vector<weightedPosition>& goalRewards)
        {
            upperBound=sizeP;

            for(auto & goalReward : goalRewards)
            {
                all_golas_data.emplace_back(short(goalReward.weightedVal),goalReward.positionPoint);
                all_golas.emplace_back(goalReward.positionPoint);
            }
        }
        Grid(const Point& upperBound_,const Point& loweBound_,vector<weightedPosition>& goalRewards)
        {
            this->upperBound=upperBound_;
            this->lowerBound=loweBound_;
            for(auto & goalReward : goalRewards)
            {
                all_golas_data.emplace_back(float(goalReward.weightedVal),goalReward.positionPoint);
                all_golas.emplace_back(goalReward.positionPoint);
            }
        }
        void print_vaule();

        vector<Point> get_goals() {
            return all_golas;
        }
        int getSizeIntGrid(){
            int size = 1;
            for (int i = 0; i < this->upperBound.capacity; ++i) {
                if (this->upperBound.array[i]==0) continue;
                size*=this->upperBound.array[i];
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

            auto pos = find_if(all_golas_data.begin(), all_golas_data.end(), [&]( pair<float,Point>& s) {
                return s.second==loc;
            });
            if (pos==all_golas_data.end())
                return false;
            if (pos->first>0)
                return true;
            return false;
        }
        float get_goal_reward(const Point& loc)
        {
            auto pos = std::find_if(all_golas_data.begin(),all_golas_data.end(),[&](pair<float,Point>& itm){
                 if(loc==itm.second)
                     return true;
                return false;
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
            return ptr_point_loc->out_of_bound(this->lowerBound,this->upperBound);
        }
        bool is_wall(const Point &loc)
        {
            return loc.out_of_bound(this->lowerBound,this->upperBound);
        }
        void setTargetGoals(const vector<bool> &vecB){
            for (int i = 0; i < vecB.size(); ++i) {
                    all_golas_data.emplace_back(vecB[i]?1:0,all_golas.operator[](i));
            }
        }
};



#endif //RACING_CAR_GRID_HPP
