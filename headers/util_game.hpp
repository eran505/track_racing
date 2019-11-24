//
// Created by ise on 14.11.2019.
//

#ifndef RACING_CAR_UTIL_GAME_HPP
#define RACING_CAR_UTIL_GAME_HPP

#include <string>
#include <list>
#include <iostream>
#include <map>
using namespace std;

enum Section{
    adversary = (int)'A',
    gurd   = (int)'B'
};

class Point{
    enum D_point{ D = 2};
public:
    ~Point() {
        //delete [] array;
            }
    Point(int x,int y);
    std::string to_str()
    {
        std::string str_to_string = "";
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
    int array[D_point::D];
    Point(){};
    Point(const Point &other):capacity(other.capacity)
    {

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=other.array[i];
        }
    }


    Point& operator+=(const Point* other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]+other->array[i];
        }
        return *this;
    }

    Point& operator-=(const Point* other){

        for (int i = 0; i < this->capacity; ++i) {
            this->array[i]=this->array[i]-other->array[i];
        }
        return *this;
    }

    bool operator== (const Point* other);

};

int range_random(int min, int max); //range : [min, max)

#endif //RACING_CAR_UTIL_GAME_HPP
