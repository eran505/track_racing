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
    vector<pair<double,Point>> all_goals_data;
    Point lowerBound=Point(0);



    public:
        void insert_goal(const Point &g,double r)
        {
            all_goals_data.emplace_back(r,g);
        }
        Point getPointSzie(){ return upperBound;}
        [[nodiscard]] const vector<pair<double,Point>>& getAllGoalsData()const{return all_goals_data;}
        explicit Grid(game_params&);
        ~Grid()=default;
        Grid()= default;
        const Point& get_lower(){return lowerBound;}
        Grid(const Point& sizeP,vector<Point> allPoint,vector<short> rewardGoal)
        {
            upperBound=sizeP;
            all_golas=std::move(allPoint);
            for(size_t i=0;i<rewardGoal.size();++i)
            {
                all_goals_data.emplace_back(rewardGoal[i],all_golas[i]);
            }
        }
        Grid(const Point& sizeP,vector<weightedPosition>& goalRewards)
        {
            upperBound=sizeP;

            for(auto & goalReward : goalRewards)
            {
                all_goals_data.emplace_back(short(goalReward.weightedVal),goalReward.positionPoint);
                all_golas.emplace_back(goalReward.positionPoint);
            }
        }
        Grid(const Point& upperBound_,const Point& loweBound_,vector<weightedPosition>& goalRewards)
        {
            this->upperBound=upperBound_;
            this->lowerBound=loweBound_;
            for(auto & goalReward : goalRewards)
            {
                all_goals_data.emplace_back(double(goalReward.weightedVal),goalReward.positionPoint);
                all_golas.emplace_back(goalReward.positionPoint);
            }
        }
        void print_vaule();

        template<typename P>
        void set_bounds(P &&lower,P &&upper)
        {
            this->lowerBound=std::forward<P>(lower);
            this->upperBound=std::forward<P>(upper);
        }
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
            for(const auto& item : all_goals_data)
            {
                if (item.second==locPoint)
                    if(item.first>0)
                        return true;
            }
            return false;
        }
        bool is_goal_reward(const Point& loc){

            auto pos = find_if(all_goals_data.begin(), all_goals_data.end(), [&]( pair<double,Point>& s) {
                return s.second==loc;
            });
            if (pos==all_goals_data.end())
                return false;
            if (pos->first>0)
                return true;
            return false;
        }
        double get_goal_reward(const Point& loc)
        {
            auto pos = std::find_if(all_goals_data.begin(),all_goals_data.end(),[&](pair<double,Point>& itm){
                 if(loc==itm.second)
                     return true;
                return false;
            });
            if (all_goals_data.end()==pos)
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
        [[nodiscard]] bool is_at_goal(const Point &loc)const {
            return (std::find(all_golas.begin(),all_golas.end(),loc) != all_golas.end());
        }
        bool is_wall(Point *ptr_point_loc)const {
            return ptr_point_loc->out_of_bound(this->lowerBound,this->upperBound);
        }
        [[nodiscard]] bool is_wall(const Point &loc)const
        {
            return loc.out_of_bound(this->lowerBound,this->upperBound);
        }
        void setTargetGoals(const vector<bool> &vecB){
            for (int i = 0; i < vecB.size(); ++i) {
                    all_goals_data.emplace_back(vecB[i]?1:0,all_golas.operator[](i));
            }
        }
        void set_upperBound(const Point& p ){upperBound=p;}
        void set_lowerBound(const Point& p ){lowerBound=p;}

};



#endif //RACING_CAR_GRID_HPP
