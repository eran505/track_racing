//
// Created by ise on 14.11.2019.
//

#include "headers/util_game.hpp"


Point::Point(int x) {
    for(int & i : array)
        i=x;
}
Point::Point(int x, int y){
    //printf("POINT_CON\n");
    this->array[0]=x;
    this->array[1]=y;
}

bool Point::operator==(const Point &other)const {
    for (int i = 0; i < Point::D ; ++i)
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





Point::Point(int x, int y, int z) {
    this->array[0]=x;
    this->array[1]=y;
    this->array[2]=z;
}


// this function return pointer to dict hash->action
std::unique_ptr<unordered_map<unsigned int,Point>> Point::getDictActionUniqie() {
    auto mapAction = std::make_unique<unordered_map<unsigned int,Point>>();
    for (int i = -1; i < 2; ++i)
        for (int j = -1; j < 2; ++j)
            for (int k = -1; k < 2; ++k) {
                auto pAction = Point(i, j, k);
                mapAction->try_emplace(pAction.hashMeAction(Point::actionMax), std::move(pAction));
            }
    return mapAction;
}


// this function return pointer to dict hash->action
unordered_map<int,Point*>* Point::getDictAction() {
    auto *mapAction = new unordered_map<int, Point *>();
    for (int i = -1; i < 2; ++i)
        for (int j = -1; j < 2; ++j)
            for (int k = -1; k < 2; ++k) {
                auto *pAction = new Point(i, j, k);
                mapAction->insert({pAction->hashMeAction(Point::actionMax), pAction});
                //cout<<pAction->hashMeAction(Point::actionMax)<<","<<"\""<<pAction->to_str()<<"\""<<endl;
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

vector<double> getTopK(int k,vector<double> &vec){
    vector<double> vectorSorted;
    vector<double> vectorTopK;
    copy(vec.begin(), vec.end(), back_inserter(vectorSorted));

    sort( vectorSorted.begin(), vectorSorted.end());
    int size = int(vectorSorted.size())-1;
    for (int i = size ; size-k <= i; --i) {
        auto it = std::find(vec.begin(), vec.end(),vectorSorted[i] );
        if (it == vec.end())
            throw ;
        else
        {
            auto index = std::distance(vec.begin(), it);
            vectorTopK.push_back(index);
            vectorTopK.push_back(vectorSorted[i]);
        }
    }
    return vectorTopK;
}
