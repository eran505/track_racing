//
// Created by ise on 13.11.2019.
//
#include "headers/Grid.hpp"

#include <utility>


void Grid::build_grid() {
    this->grid = new string*[this->x_size];
    for (int i = 0; i < this->x_size; ++i)
    {
        this->grid[i] = new string[this->y_size];
    }
}

Grid::~Grid() {
    for (auto i : *(this->all_golas)) {
        delete(i);
    }
    delete(this->all_golas);
}

std::list<std::tuple<uint, uint>> Grid::get_goals() {
    return std::list<std::tuple<uint, uint>>();
}



void Grid::init_grid(const game_params& parm) {
    this->x_size=parm.X;
    this->y_size=parm.Y;
    this->all_golas=parm.list_goals;
}

Grid::Grid() {

}




void Grid::print_vaule() {
    cout<<"X:\t"<<this->x_size<<endl;;
    cout<<"Y:\t"<<this->y_size<<endl;;
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

void box::set_agent(string &str_agent) {
    this->info = str_agent;
}

ostream &operator<<(ostream &os,  const box& my_box) {
    return os << "|" << my_box.info<<" |" << endl;

}


