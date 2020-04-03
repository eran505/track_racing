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

Grid::~Grid() {
    for (auto i : *(this->all_golas)) {
        delete(i);
    }
    delete(this->all_golas);
}



void Grid::print_vaule() {

    cout<<"grid dimensions:\t";
    cout<<this->size_point.to_str()<<endl;
    cout<<"Goals:\t";
    reverse_iterator<list<Point *>::iterator> revIt;
    int size_list_goal=this->all_golas->size();
    int ctr=0;
    for (auto const& i : *(this->all_golas)) {
        if (ctr==size_list_goal-1)
            std::cout << i->to_str()<<endl;
        else
            std::cout << i->to_str()<<", ";
        ctr++;
    }


}

Grid::Grid(const game_params &parm) {
//
//    this->x_size=parm.X;
    this->size_point=parm.size;
    this->all_golas=parm.list_goals;
}

void box::set_agent(string &str_agent) {
    this->info = str_agent;
}

ostream &operator<<(ostream &os,  const box& my_box) {
    return os << "|" << my_box.info<<" |" << endl;

}


