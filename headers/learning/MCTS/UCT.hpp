//
// Created by ERANHER on 4/30/20.
//

#ifndef TRACK_RACING_UCT_HPP
#define TRACK_RACING_UCT_HPP
#define DEBUG

#include <utility>
#include "Policy/Policy.hpp"
#ifdef DEBUG
#endif

/*
 * ref https://github.com/PetterS/monte-carlo-tree-search/blob/master/mcts.h
 * ref https://github.com/steve3003/mcts-cpp/blob/master/mcts-cpp/Src/MCTSAlgorithm.cpp
 *
 *
 * */
namespace MCTS{
    template<typename StateG>
    class Node{
    public:
        typedef typename StateG::Action action;

        explicit Node(const State& state);
        explicit Node(State&& state) noexcept; //for move copy
        Node& operator=(Node&& rhs) noexcept;

        ~Node();
        bool has_untried_moves() const;
        template<typename RandomEngine>
        action get_untried_move(RandomEngine* engine) const;
        Node* best_child() const;

        bool has_children() const
        {
            return ! children.empty();
        }

        Node* selectChildUct() const;
        Node* add_child(const action& move, const State& state);
        void update(double result);

        std::string to_string() const;
        std::string tree_to_string(int max_depth = 1000000, int indent = 0) const;

        const action move;
        Node* const parent;
        const int player_to_move;

        //std::atomic<double> wins;
        //std::atomic<int> visits;
        double value;
        unsigned int visits;
        int win;
        std::vector<action> moves;
        std::vector<Node*> children;
        double UCT_score;

    private:
        void MoveFrom(const Node& src);


    };

    template<typename StateG>
    Node<StateG>::Node(const State &state) {

    }
    template<typename StateG>
    Node<StateG>::Node( State &&state) noexcept{

    }
    template<typename StateG>
    Node<StateG>* Node<StateG>::selectChildUct() const
    {
        attest( ! children.empty() );
        for (auto child: children) {
            child->UCT_score     = double(child->wins) / double(child->visits) +
                               std::sqrt(2.0 * std::log(double(this->visits)) / child->visits);
        }

        return *std::max_element(children.begin(), children.end(),
                                 [](Node* a, Node* b) { return a->UCT_score < b->UCT_score; });
    }

    template<typename StateG>
    Node& Node<StateG>::operator=(Node &&rhs) noexcept {
        // Check for self-assignment
        if (this == &rhs) {
            return *this;
        }
        // Shallow copy of data
        MoveFrom(rhs);
        return *this;
    }
}

#undef DEBUG
#endif //TRACK_RACING_UCT_HPP
