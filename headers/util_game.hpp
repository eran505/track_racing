//
// Created by ise on 14.11.2019.
//

#ifndef RACING_CAR_UTIL_GAME_HPP
#define RACING_CAR_UTIL_GAME_HPP

#include <string>
#include <list>
#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <set>
using namespace std;

enum Section{
    adversary = (int)'A',
    gurd   = (int)'B'
};

class Point{
    enum D_point{ D = 2};
public:
    ~Point() = default;
    Point(int x,int y);
    std::string to_str()
    {
        std::string str_to_string;
        str_to_string+="(";
        for ( int i = 0; i < this->capacity-1; i++) {
            str_to_string += std::to_string(*(this->array+ sizeof(int)*i));
            str_to_string += ", ";
        }
        str_to_string+= std::to_string(*(array + this->capacity-1));
        str_to_string+=')';
        return str_to_string;
    }
    int capacity=D_point::D;
    int array[D_point::D]{};
    Point(){//printf("POINT_EMPTY_CON\n");
         };
    Point(const Point &other):capacity(other.capacity)
    {
        //printf("COPY_CON POINT\n  ");
        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=other.array[i];
        }
    }

    void operator+=(const Point &other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]+other.array[i];
        }

    }
    int hash2D() const
    {
        return  (53 + (array[0])) * 53 + (array[1]);
    }
//    int hash3D(){
//        return  ((53 +  array[0]) * 53 +  array[1]) * 53 + array[2];
//    }



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
    bool is_equal(const Point *otehr)const;
};

int range_random(int min, int max); //range : [min, max)



#endif //RACING_CAR_UTIL_GAME_HPP
