//
// Created by ise on 14.11.2019.
//

#ifndef RACING_CAR_UTIL_GAME_HPP
#define RACING_CAR_UTIL_GAME_HPP

#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <climits>
#include <unordered_map>
#include <random>
#include <cmath>
#include <map>
#include <algorithm>
#include <set>
#include <memory>
#include <queue>
using std::vector;
using std::map;
using std::unordered_map;
using std::cout;
using std::list;
using std::ios_base;using std::multimap;
using std::pair;using std::shared_ptr;
using std::reverse_iterator;using std::tuple;
using std::ostream;using std::set;
using std::string;
using std::endl;
using std::queue;


enum Section{
    adversary = (int)'A',
    gurd   = (int)'D'
};

class Point{

public:
    enum D_point{
        D = 3, actionMax=3, maxSpeed=5
    };
    ~Point() = default;
    Point(int x);
    Point(int x,int y);
    Point(int x,int y,int z);
    std::string to_str() const {
        std::string str_to_string;
        str_to_string+="(";
        for ( int i = 0; i < this->capacity-1; i++) {
            str_to_string += std::to_string((this->array[i]));
            str_to_string += ", ";
        }
        str_to_string+= std::to_string(*(array + this->capacity-1));
        str_to_string+=')';
        return str_to_string;
    }
    string to_hash_str() const {
        string str;
        for (int i = 0; i < this->capacity; ++i) {
            str += std::to_string(this->operator[](i));
        }
        return str;
    }
    int multi(){
        int res=1;
        for (int i = 0; i < this->capacity; ++i) {
            res*=this->array[i];
        }
        return res;
    }
    int sumPoint()
    {
        int sum=0;
        for (int i = 0; i < this->capacity; ++i) {
            sum+=this->array[i];
        }
        return sum;
    }
    int capacity=D_point::D;
    int array[D_point::D]{};
    Point(){ //printf("POINT_EMPTY_CON\n");
    };
    Point(const Point &other):capacity(other.capacity)
    {
        //printf("COPY_CON POINT\n  ");
        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=other.array[i];
        }
    }

    Point& operator=(const Point &other){
        //printf("operator= Point \n  ");
        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=other.array[i];
        }
        return *this;
    }


    void operator+=(const Point &other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]+other.array[i];
        }

    }
    bool pairIsEqule(pair<int,int> data)
    {
        if(this->array[0]!=data.first or this->array[1]!=data.second)
            return false;
        return true;
    }
    int hash2D() const
    {
        return  (53 + (array[0])) * 53 + (array[1]);
    }
//    int hash3D(){
//        return  ((53 +  array[0]) * 53 +  array[1]) * 53 + array[2];
//    }

    Point operator-(const Point* other) const{
        Point newp(0);
        for (int i = 0; i < this->capacity; ++i) {
            newp.array[i]=this->array[i]-other->array[i];
        }
        return newp;
    }


    Point& operator-=(const Point* other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]-other->array[i];
        }
        return *this;
    }
    bool any_bigger_equle(const Point &other){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]<=other.array[i])
                return true;
        }
        return false;
    }
    bool out_of_bound(const Point &bound){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]>=bound.array[i] or this->array[i]<0)
                return true;
        }
        return false;
    }

    vector<float>* getFeature()
    {
        auto* vectorI = new vector<float>();
        for (int i = 0; i < this->capacity; ++i)
            vectorI->push_back(this->array[i]);

        return vectorI;
    }
    bool any_ngative(){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]<0)
                return true;
        }
        return false;
    }
    void change_speed_max(int absoult_max){
        for (int i = 0; i < this->capacity; ++i) {
            if (this->array[i]>absoult_max){
                this->array[i]=absoult_max;
                continue;
            }
            if(this->array[i]*-1>absoult_max)
                this->array[i]=absoult_max*-1;
        }
    }
    bool operator== (const Point &other);
    bool is_equal(const Point *other)const;
    int operator[](int index)const;

    static vector<Point> getAllAction(vector<Point> &action_list){
        action_list.clear();
        if (Point::D==2){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    action_list.emplace_back(i,j);
        }else if(Point::D==3){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    for (int k = -1; k < 2; ++k)
                        action_list.emplace_back(i,j,k);}
        return action_list;
    }

    static vector<Point> getAllAction2(vector<Point> &action_list,int x=-1){
        action_list.clear();
        if (Point::D==2){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    action_list.emplace_back(i,j);
        }else if(Point::D==3){
            for (int i = -1; i < 2; ++i)
                for (int j = -1; j < 2; ++j)
                    for (int k = -1; k < 2; ++k)
                    {
                        if (x==0)
                            action_list.emplace_back(0,j,k);
                        if (x==1)
                            action_list.emplace_back(i,0,k);
                        if (x==2)
                            action_list.emplace_back(i,j,0);
                    }

                        }
        return action_list;
    }


    int hashConst(int offset=0)const {
        double h=hashNnN(array[0]+offset,array[1]+offset);
        for (int i = 2; i < capacity; ++i) {
            h=hashNnN(int(h),this->array[i]+offset);
        }
        return int(h);
    }
    static unordered_map<int,Point*>* getDictAction();
    Point operator-(const Point &other) const
    {
        Point newP;
        for (int i = 0; i < capacity; ++i) {
            newP.array[i]=this->array[i]-other.array[i];
        }
        return newP;
    }
    unsigned int hashMeAction(int max){
        unsigned int h=0;
        int append=int(pow(int(actionMax),this->capacity))/2;
        for (int i = 0; i < this->capacity; ++i)
            h+=this->array[capacity-1-i]*int(pow(max,i));
        return h+append;
    }

    bool isOK(){
        bool ok = true;
        for (int i = 0; i < this->capacity; ++i)
            if (array[i]>1 or array[i]<-1)
                return false;
        return ok;
    }

    unsigned long expHash()const {
        std::size_t seed = capacity;
        for(auto& i : array) {
            seed ^= i + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        }
        return seed;
    }
    unsigned int hashMeMAX(int max){
        unsigned int h=0;
        //int append=int(pow(int(actionMax),this->capacity))/2;
        for (int i = 0; i < this->capacity; ++i)
            h+=this->array[capacity-1-i]*int(pow(max,i));
        return h;
    }

    static int hashNnN(int x,int y){ return int(((x+y)*(x+y+1))*0.5+y);}
};


int getMaxDistance(const Point &a , const  Point &b );
int getMaxDistancePos(const Point &a , const  Point &b );
int range_random(int min, int max); //range : [min, max)
vector<float> getTopK(int k,vector<float> &vec);


template <typename T>

bool Contains( std::vector<float>& Vec, const float Element )
{
    if (std::find(Vec.begin(), Vec.end(), Element) != Vec.end())
        return true;

    return false;
}


#endif //RACING_CAR_UTIL_GAME_HPP
