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
MdpPlaner* init_mdp(Grid *g);


int main() {

    auto g= init_grid(10,10);
    //g->print_vaule();

    auto pPlaner = init_mdp(g);

    pPlaner->set_grid(g);
    pPlaner->get_player("B4") == nullptr ? cout << "ya\n" : cout << "no\n";


    Game* my_game = new Game(pPlaner);
    my_game->init_game();

    my_game->fill_agents();
    my_game->reset_game();
    //my_game->print_list_in_game();
    cout<<"------LOOP GAME!!------"<<endl;
    my_game->loop_game();
    delete(my_game);
    cout<<"------END MAIN!!------"<<endl;

    return 0;

}

Grid * init_grid(int x, int y){
    game_params m{};
    auto m_ofList = new list<Point *>;
    m_ofList->push_front(new Point(5,5));
    m_ofList->push_front(new Point(1,1));
    m.size=Point(x,y);
    m.list_goals=m_ofList;
    Grid *g = new Grid(m);
    return g;
}
MdpPlaner* init_mdp(Grid *g){

    auto* a1 = new Agent(new Point(1,6)
            ,new Point(0,0)
            ,adversary,10);

    auto* b1 = new Agent(new Point(1,4),
            new Point(0,0)
            ,gurd,10);

    auto* b2 = new Agent(new Point(6,6),new Point(0,0)
            ,gurd,10);

    Policy *p_polcy_a = new Dog("Dog",3);
    Dog *d = dynamic_cast <Dog*>(p_polcy_a);
    d->set_move(0);

    Policy *p_polcy_b1 = new Dog("Dog",3,0);
    Policy *p_polcy_b2 = new Dog("Dog",3,0);
    p_polcy_b1->add_tran(p_polcy_a);
    p_polcy_b2->add_tran(p_polcy_a);
    a1->set_policy(p_polcy_a);
    b2->set_policy(p_polcy_b2);
    b1->set_policy(p_polcy_b1);

    auto* s = new MdpPlaner();
    s->add_player(a1);
    s->add_player(b1);
    s->add_player(b2);
    s->set_grid(g);
    s->set_state();


    return s;
}