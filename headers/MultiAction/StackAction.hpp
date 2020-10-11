//
// Created by ERANHER on 29.7.2020.
//

#ifndef TRACK_RACING_STACKACTION_HPP
#define TRACK_RACING_STACKACTION_HPP
#include "State.hpp"
namespace Actionzer {
    struct tuple_stack {
        State state;
        Point action;
        u_int64_t entryID;

        tuple_stack(State state_val,Point action_val,u_int64_t entryID_val)
        :state(std::move(state_val)),action(std::move(action_val)),entryID(entryID_val){}
        string to_string() const {
            return "{ " + state.to_string_state() + " , " + action.to_str()+ " }";
        }


    };

    template<typename S>
    class StackActionzer {

        std::vector<S> stack;
        u_int32_t ctr_stack=0;
    public:
        void inset_to_stack(S &&item) {
            stack.emplace_back(item);
            ctr_stack++;
        }
        bool is_empty()
        {
            return ctr_stack==0 ? true : false;
        }
        S& pop() {
            return stack[--ctr_stack];
        }
        State get_last_state(){
            assert(!stack.empty());
            return static_cast<tuple_stack>(stack.front()).state;
        }
        Point get_last_action(){
            assert(!stack.empty());
            return static_cast<tuple_stack>(stack.front()).action;
        }
        void clear()
        {
            stack.clear();
            ctr_stack=0;
        }
        void print_stak()
        {
            if(is_empty())
            {
                cout<<"{}"<<endl;
                return;
            }
            for(int i=stack.size()-1;i>=0;--i)
            {
                cout<<" [stack] ["<<i<<"] "<<stack[i].to_string()<<"\n";
            }
            cout<<endl;
        }


    };
}
#endif //TRACK_RACING_STACKACTION_HPP
