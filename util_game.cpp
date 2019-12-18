//
// Created by ise on 14.11.2019.
//

#include "headers/util_game.hpp"


Point::Point(int x, int y){
    //printf("POINT_CON\n");
    this->array[0]=x;
    this->array[1]=y;
}

bool Point::operator==(const Point &other) {
    for (int i = 0; i <this->capacity; ++i)
        if (this->array[i]!=other.array[i])
            return false;
    return true;
}

bool Point::is_equal(const Point *otehr) const{
    for (int i = 0; i <this->capacity; ++i)
        if (this->array[i]!=otehr->array[i])
            return false;
    return true;
}




int range_random(int min, int max) //range : [min, max)
{
    static bool first = true;
    if (first)
    {
        cout<<"in"<<endl;
        srand( time(NULL) ); //seeding for the first time only!
        first = false;
    }
    return min + rand() % (( max + 1 ) - min);
}

