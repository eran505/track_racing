//
// Created by ise on 13.11.2019.
//
#include "headers/Grid.hpp"

#include <utility>


//void Grid::build_grid() {
//    this->grid = new string*[this->x_size];
//    for (int i = 0; i < this->x_size; ++i)
//    {
//        this->grid[i] = new string[this->y_size];
//    }
//}





void Grid::print_vaule() {

    cout<<"grid dimensions:\t";
    cout<<this->upperBound.to_str()<<endl;
    cout<<"Goals:\t";
    reverse_iterator<list<Point *>::iterator> revIt;
    int size_list_goal=this->all_golas.size();
    int ctr=0;
    for (auto const& i : this->all_golas) {
        if (ctr==size_list_goal-1)
            std::cout << i.to_str()<<endl;
        else
            std::cout << i.to_str()<<", ";
        ctr++;
    }


}

Grid::Grid(game_params &parm) {
//
//    this->x_size=parm.X;
    this->upperBound=parm.size;
    this->all_golas=std::move(parm.list_goals);
}



