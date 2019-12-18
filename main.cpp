#include <iostream>
#include "headers/Grid.hpp"
#include "headers/Agent.hpp"
#include <list>
#include "headers/util_game.hpp"
#include "headers/MdpPlaner.hpp"
#include "headers/Game.hpp"
#include "headers/Policy/Dog.hpp"
#include "headers/graph/graph_util.hpp"
#include <vector>
#include<algorithm>
#include "Policy/RtdpAlgo.hpp"
#include <chrono>
Grid * init_grid(int x, int y);
MdpPlaner* init_mdp(Grid *g);


list<int> arg_max2(const double arr[],int size ){
    double max = *std::max_element(arr, arr+size);
    list<int> l;
    for (int i = 1; i < size; ++i) {
        if (arr[i]==max)
            l.push_front(i);
    }
    return l;
}


int main() {

    std::chrono::steady_clock::time_point begin2 = std::chrono::steady_clock::now();
    std::chrono::steady_clock::time_point end2 = std::chrono::steady_clock::now();
    std::cout << "Time difference = " << std::chrono::duration_cast<std::chrono::microseconds>(end2 - begin2).count() << "[µs]" << ':';
    std::cout << " " << std::chrono::duration_cast<std::chrono::nanoseconds> (end2 - begin2).count() << "[ns]" << std::endl;

    auto g= init_grid(9,9);
    //g->print_vaule();

    auto pPlaner = init_mdp(g);

    pPlaner->set_grid(g);
    pPlaner->get_player("B4") == nullptr ? cout << "ya\n" : cout << "no\n";


    Game* my_game = new Game(pPlaner);
    cout<<"------LOOP GAME!!------"<<endl;
    my_game->start_game(1);
    delete(my_game);
    cout<<"------END MAIN!!------"<<endl;

    return 0;

}

Grid * init_grid(int x, int y){
    game_params m{};
    auto m_ofList = new list<Point *>;
    m_ofList->push_front(new Point(0,0));
    m_ofList->push_front(new Point(1,0));
    m.size=Point(x,y);
    m.list_goals=m_ofList;
    Grid *g = new Grid(m);
    return g;
}
MdpPlaner* init_mdp(Grid *g){

    auto* a1 = new Agent(new Point(8,9)
            ,new Point(0,0)
            ,adversary,10);

//    auto* a2 = new Agent(new Point(3,4),
//            new Point(-1,-1)
//            ,adversary,10);

    auto* b2 = new Agent(new Point(4,5),new Point(0,0)
            ,gurd,10);

    Policy *p_polcy_a1 = new Dog("Dog",1);
    Dog *d = dynamic_cast <Dog*>(p_polcy_a1);
    d->set_goal(0,0);
    d->set_goal(1,0);

    //Policy *p_polcy_a2 = new Dog("Dog",3,0);

    //// init the RTDP algo
    list<pair<int,int>> l;
    l.emplace_back(2,1);
    l.emplace_back(2,1);
    Policy *RTDP = new RtdpAlgo("RTDP",2,g->getSizeIntGrid(),l);
    RTDP->add_tran(p_polcy_a1);
    //RTDP->add_tran(p_polcy_a2);


    p_polcy_a1->add_tran(p_polcy_a1);
    //p_polcy_a2->add_tran(p_polcy_a2);
    a1->setPolicy(p_polcy_a1);
    b2->setPolicy(RTDP);
    //a2->setPolicy(p_polcy_a2);

    auto* s = new MdpPlaner();
    s->add_player(a1);
    //s->add_player(a2);
    s->add_player(b2);
    s->set_grid(g);
    s->set_state();


    return s;
}