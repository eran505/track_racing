//
// Created by ERANHER on 11.3.2020.
//

#ifndef TRACK_RACING_SUMTREE_HPP
#define TRACK_RACING_SUMTREE_HPP

#include <utility>
#include <limits>
#include "../../util_game.hpp"
#include "../../State.hpp"
#include <functional>

typedef vector<float> feature;
class experienceTuple{
public:
    short aAction;
    vector<feature*> ptrNextStateVec;
    feature* ptrState;
    vector<short> isEndStateNot;
    feature* ptrProbabilities;
    feature* ptrRewards;

    experienceTuple(short action, feature *state, vector<short> &isNotEnd,
                    feature *prob,feature *reward,vector<feature*> &nextState): aAction(action),
                    ptrRewards(reward),ptrProbabilities(prob),isEndStateNot(std::move(isNotEnd)),ptrState(state){
        for(const auto i : nextState)
        {
            ptrNextStateVec.push_back(i);
        }
    }
    ~experienceTuple(){
        for(auto item : ptrNextStateVec)
            delete item;
        delete(ptrRewards);
        delete(ptrProbabilities);
        delete(ptrState);
    }
};
/** ------ Unsorted Sum Tree ----
 * a binary tree data structure where the parent’s value is the sum of its children.
 * The samples themselves are stored in the leaf nodes.
 * A tree which can be used as a min/max heap or a sum tree
 * Add or update item value - O(log N)
 * Sampling an item - O(log N)
 * */

float maxFunc(float a ,float b){ return max(a,b);}
float minFunc(float a ,float b){ return min(a,b);}
float addFunc(float a ,float b){ return a+b;}
enum operationTree{
    maxTree = 1,
    minTree = 2,
    addTree = 3,
};
typedef vector<experienceTuple*> experiences;
class SumTree{

    unsigned int write;
    unsigned int capacity;
    experiences dataTree;
    vector<float> tree;
    std::function<float(float,float)> foo;


    /**
     Propagate an update of a node's value to its parent node
     param node_idx: the index of the node that was updated
     */
    void propagate(unsigned int idx){
        auto parent = (idx -1) / 2 ;
        this->tree[parent] = foo(tree[parent * 2 + 1],tree[parent * 2 + 2]);
        if (parent != 0)
            this->propagate(parent);
    }

    /**
     *Retrieve the first node that has a value larger than val and is a child of the node at index idx
      :param root_node_idx: the index of the root node to search from
      :param val: the value to query for
      :return: the index of the resulting node
     * **/
    unsigned int retrieve(unsigned int root_node_idx, float val){
        auto left = 2 * root_node_idx + 1;
        auto right = left + 1;
        if (left>=this->tree.size())
            return root_node_idx;
        if (val <= this->tree[left] )
            return this->retrieve(left,val);
        else
            return this->retrieve(right,val - this->tree[left]);
    }
public:
    explicit SumTree(unsigned int _capacity,operationTree kind):capacity(_capacity),write(0)
    {
        float initialValue=0;
        if (!((capacity & (capacity - 1)) == 0))
            throw std::invalid_argument( "A segment tree size must be a positive power of 2." );

        if (kind == maxTree){
            initialValue=std::numeric_limits<float>::max();
            foo=maxFunc;
        }else if(kind == minTree){
            initialValue=std::numeric_limits<float>::min();
            foo=minFunc;
        }else if(kind == addTree){
            foo=addFunc;
        }else
            throw std::invalid_argument( "the operation Tree is invalid" );
        // initial
        this->tree =vector<float>(2*capacity-1,initialValue);
        this->dataTree.reserve(capacity);
    }
    ~SumTree(){
        for(auto item: dataTree)
            delete(item);
    }
    /**get the object **/
    experienceTuple* getData(unsigned int idx){
        return dataTree[idx];
    }
    /**get the probability **/
    float getTreeValue(unsigned int idx){
        return tree[idx];
    }
    /**
     Add a new value to the tree with data assigned to it
     :param val: the new value to add to the tree
     :param data: the data that should be assigned to this value
     **/
    void add(float p , experienceTuple *ptrExp)
    {
        auto ptrOld = this->dataTree[write];
        delete ptrOld;
        this->dataTree[write]=ptrExp;
        //auto idx= this->write + this->capacity - 1;
        this->update(write,p);
        this->write++;
        if (write >= capacity)
            write=0;
    }
    /**
    Given a value between 0 and the tree sum, return the object which this value is in it's range.
    For example, if we have 3 leaves: 10, 20, 30, and val=35, this will return the 3rd leaf, by accumulating
    leaves by their order until getting to 35. This allows sampling leaves according to their proportional
    probability.
    :param val: a value within the range 0 and the tree sum
    :return: the index of the resulting leaf in the tree,
     its probability and the object itself
    **/
    tuple<unsigned int,unsigned int>  getElementByPartialSum(float s){
        auto idxTree= this->retrieve(0, s);
        auto idxData = idxTree - this->capacity + 1;
        return {idxTree,idxData};
    }
    /**
     Return the total value of the tree according to the tree operation. For SUM for example, this will return
     the total sum of the tree. for MIN, this will return the minimal value
     :return: the total value of the tree
      **/
    float total(){ return this->tree[0]; }
    /**
 Update the value of the node at index idx
 :param leaf_idx: the index of the node to update
 :param new_val: the new value of the node
 **/
    void update(unsigned int idx,float p)
    {
        auto node_idx = idx + capacity - 1;
        if (node_idx<0)
            throw std::invalid_argument( "Error: node_idx out of lim ( 0 > node_idx ) ");
        if (node_idx>= capacity)
            throw std::invalid_argument( "Error: node_idx out of lim ( 0 > capacity )");
        this->tree[node_idx]=p;
        this->propagate(node_idx);
    }
};




#endif //TRACK_RACING_SUMTREE_HPP
