#include <iostream>
#include "headers/Grid.hpp"
#include "headers/Agent.hpp"
#include <list>
#include "headers/util_game.hpp"
#include "headers/MdpPlaner.hpp"
#include "headers/Game.hpp"
#include "headers/Policy/Dog.hpp"
typedef unsigned int uint;
Grid * init_grid(int x, int y);
MdpPlaner* init_mdp();
int main() {


    auto g= init_grid(10,10);
    g->print_vaule();

    auto pPlaner = init_mdp();
    pPlaner->set_grid(g);
    pPlaner->get_player("B4") == nullptr ? cout << "ya\n" : cout << "no\n";

    cout<<"END MAIN!!"<<endl;
    Game* my_game = new Game(pPlaner);
    my_game->init_game();
    my_game->fill_agents();
    my_game->print_list_in_game();
    my_game->loop_game();
    delete(my_game);

    return 0;

}

Grid * init_grid(int x, int y){
    game_params m;
    auto m_ofList = new list<Point *>;
    m_ofList->push_front(new Point(5,5));
    m_ofList->push_front(new Point(0,0));
    m.X=x;
    m.Y=y;
    m.list_goals=m_ofList;
    Grid *g = new Grid();
    g->init_grid(m);
    return g;
}
MdpPlaner* init_mdp(){

    auto* a1 = new Agent(new Point(range_random(0,10),range_random(0,10))
            ,new Point(range_random(-1,1),range_random(-1,1))
            ,adversary,10);

    auto* b1 = new Agent(new Point(range_random(0,10),range_random(0,10)),new Point(range_random(-1,1),range_random(-1,1))
            ,gurd,10);

    auto* b2 = new Agent(new Point(range_random(0,10),range_random(0,10)),new Point(range_random(-1,1),range_random(-1,1))
            ,gurd,10);

    Policy *p_polcy_a = new Dog("A1","Dog");
    Dog *d = dynamic_cast <Dog*>(p_polcy_a);
    d->set_move(1);

    Policy *p_polcy_b = new Dog("B1","Dog",-1);
    a1->set_policy(p_polcy_a);
    b1->set_policy(p_polcy_b);
    b2->set_policy(p_polcy_b);

    MdpPlaner* s = new MdpPlaner();
    s->add_player(a1);
    s->add_player(b1);
    s->add_player(b2);
    s->set_state();
    return s;
}