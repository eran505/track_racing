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

bool Point::is_equal(const Point *other) const{
    for (int i = 0; i <this->capacity; ++i)
        if (this->array[i]!=other->array[i])
            return false;
    return true;
}

int Point::operator[](int index) const{
    return this->array[index];
}

Point::Point(int x, int y, int z) {
    this->array[0]=x;
    this->array[1]=y;
    this->array[2]=z;
}


// this function return pointer to dict hash->action
unordered_map<int,Point*>* Point::getDictAction() {
    auto *mapAction = new unordered_map<int, Point *>();
    if (Point::D == 2) {
        for (int i = -1; i < 2; ++i)
            for (int j = -1; j < 2; ++j) {
                auto *p_action = new Point(i, j);
                mapAction->insert({p_action->hashMeAction(Point::actionMax), p_action});
            }
    } else if (Point::D == 3) {
        for (int i = -1; i < 2; ++i)
            for (int j = -1; j < 2; ++j)
                for (int k = -1; k < 2; ++k) {
                    auto *pAction = new Point(i, j, k);
                    mapAction->insert({pAction->hashMeAction(Point::actionMax), pAction});
                }
    }
    return mapAction;
}

int range_random(int min, int max) //range : [min, max)
{
    static bool first = true;
    if (first)
    {
        //srand( time(NULL) ); //seeding for the first time only!
        first = false;
    }
    return min + rand() % (( max + 1 ) - min);
}

int getMaxDistance(const  Point &a , const  Point &b )
{
    int max=-1;
    for (int i = 0; i < a.capacity; ++i)
    {
        auto dis = abs(a[i]-b[i]);
        if (dis>max) max = dis;

    }
    return max;
}
int getMaxDistancePos(const Point &a , const  Point &b )
{
    int max=-1;
    int pos = -1;
    for (int i = 0; i < a.capacity; ++i)
    {
        auto dis = abs(a[i]-b[i]);
        if (dis>max){
            max = dis;
            pos=i;
        }

    }
    return pos;
}