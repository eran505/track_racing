//
// Created by eranhe on 1/16/21.
//

#ifndef TRACK_RACING_TREESUM_HPP
#define TRACK_RACING_TREESUM_HPP
#include "torch/torch.h"

#include "headers/learning/ReplayBuffer/Featuerzer.hpp"







/** ------ Unsorted Sum Tree ----
 * a binary tree data structure where the parent’s value is the sum of its children.
 * The samples themselves are stored in the leaf nodes.
 * A tree which can be used as a min/max heap or a sum tree
 * Add or update item value - O(log N)
 * Sampling an item - O(log N)
 * */
double maxFunc(double a ,double b){ return std::max(a,b);}
double minFunc(double a ,double b){ return std::min(a,b);}
double addFunc(double a ,double b){ return a+b;}
enum operationTree{
    maxTree = 1,
    minTree = 2,
    addTree = 3,
};

typedef unordered_map<unsigned long,unsigned int> IndexesDict;

template <typename T>
class SumTree{
    typedef double metric;
public:
    unsigned int write;
    unsigned int capacity;
    unsigned int ctrDebug=0;
    unsigned int treeSize;
    bool dup;
    std::vector<T> dataTree;
    IndexesDict mIndexesDict;
    vector<metric> tree;
    std::function<metric(metric,metric)> foo;


    void toStringTree()
    {
        short ctr=0;
        cout<<"dataTree-size(),"<<dataTree.size()<<endl;
        for(const auto &val:tree)
        {
            cout<<ctr<<","<<val<<endl;
            ctr++;
        }
    }
    unsigned int retrieve2(unsigned int root_node_idx, metric val){
        unsigned int leafIdx;
        unsigned int parentIndex=root_node_idx;
        while(true)
        {
            auto left = 2 * parentIndex + 1;
            auto right = left + 1;
           // cout<<"left: "<<left<<"\tright: "<<right<<"\tval: "<<val<<endl;
            if (left>=this->tree.size())
            {
                leafIdx = parentIndex;
                break;
            }
            else{
                if(val <= this->tree[left])
                    parentIndex = left;
                else{
                     val = val - this->tree[left];
                    parentIndex = right;
                }
            }

        }
        return leafIdx;
    }

    /**
     *Retrieve the first node that has a value larger than val and is a child of the node at index idx
      :param root_node_idx: the index of the root node to search from
      :param val: the value to query for
      :return: the index of the resulting node
     * **/
    unsigned int retrieve(unsigned int root_node_idx, metric val){
        auto left = 2 * root_node_idx + 1;
        auto right = left + 1;
        if (left>=this->tree.size())
            return root_node_idx;
        if (val <= this->tree[left] )
            return this->retrieve(left,val);
        else
            return this->retrieve(right,val - this->tree[left]);
    }

    explicit SumTree(unsigned int _capacity,operationTree kind, bool duplication = false):capacity(_capacity),write(0),treeSize(0)
    ,dataTree(capacity)
    {
        dup=duplication;
        double initialValue=0;
        if ((capacity & (capacity - 1)) != 0)
            throw std::invalid_argument( "A segment tree size must be a positive power of 2." );

        if (kind == maxTree){
            initialValue=std::numeric_limits<double>::max();
            foo=maxFunc;
        }else if(kind == minTree){
            initialValue=std::numeric_limits<double>::min();
            foo=minFunc;
        }else if(kind == addTree){
            foo=addFunc;
        }else
            throw std::invalid_argument( "the operation Tree is invalid" );
        // initial
        this->tree =vector<metric>(2*capacity-1,initialValue);
        this->treeSize=2*capacity-1;
        //this->dataTree = experiences(capacity);
        if (!duplication)
            this->mIndexesDict.reserve(capacity);
    }

    /**get the object **/
    T getData(unsigned int idx){
        return dataTree[idx];
    }
    /**get the probability **/
    metric getTreeValue(unsigned int idx){
        return tree[idx];
    }
    void delIndexesDict(T ptrOld){
        auto hVal = ptrOld->hashValue();
        auto pos = mIndexesDict.find(hVal);
        if ( pos == mIndexesDict.end())
            throw std::invalid_argument( "cant find the key in mIndexesDict");
        int index = pos->second;
        mIndexesDict.erase(hVal);

    }
    bool isContain(unsigned long h){
        auto pos = mIndexesDict.find(h);
        if (pos == mIndexesDict.end())
            return false;
        return true;
    }
    /**
     Add a new value to the tree with data assigned to it
     :param val: the new value to add to the tree
     :param data: the data that should be assigned to this value
     **/
    void add(metric p , T ptrExp)
    {
        //std::setprecision(10);
        if(dup)
        {
            T ptrOld = this->dataTree[write];
            this->dataTree[write]=ptrExp;
        }
        else
        {
            u_int64_t hashValue=0;



            hashValue = ptrExp.hashValue();
            if (isContain(hashValue)) {
                return;
            }

            T ptrOld = this->dataTree[write];
//            if (ptrOld != nullptr)
//                this->delIndexesDict(ptrOld);
            this->mIndexesDict.insert({hashValue,write});
            this->dataTree[write]=ptrExp;
        }

        //auto idx= this->write + D - 1;
       // cout<<"P: "<<p<<endl;
        this->update(write,p);
        this->write++;
        this->ctrDebug++;
        if (write >= capacity)
            write=0;
        //cout<<"write: "<<write<<endl;
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
    tuple<unsigned int,unsigned int>  getElementByPartialSum(metric s){
        auto idxTree= this->retrieve2(0, s);
        auto idxData = idxTree - 3 + 1;
        return {idxTree,idxData};
    }
    /**
     Return the total value of the tree according to the tree operation. For SUM for example, this will return
     the total sum of the tree. for MIN, this will return the minimal value
     :return: the total value of the tree
      **/
    double total(){ return this->tree[0]; }
    /**
 Update the value of the node at index idx
 :param leaf_idx: the index of the node to update
 :param new_val: the new value of the node
 **/
    void update(unsigned int idx,metric p)
    {
        auto node_idx = idx + capacity - 1; //Look at what index we want to put the experience
        if (node_idx<0)
            throw std::invalid_argument( "Error: node_idx out of lim ( 0 > node_idx ) ");
        if (node_idx >= treeSize)
            throw std::invalid_argument( "Error: node_idx out of lim ( 0 > capacity )");
        //auto change = p - tree[node_idx]

        auto change = p - this->tree[node_idx];
        this->tree[node_idx]=p;

        while (node_idx != 0)
        {
            //cout<<"update\n";
            node_idx = (node_idx - 1)/ 2;
            this->tree[node_idx] += change;
            //this->tree[node_idx] = foo(change,tree[node_idx]) ;

        }
    }

    /**
 Propagate an update of a node's value to its parent node
 param node_idx: the index of the node that was updated
 */
//    void propagate(unsigned int idx){
//        auto parent = (idx -1) / 2 ;
//        this->tree[parent] = foo(tree[parent * 2 + 1],tree[parent * 2 + 2]);
//        if (parent != 0)
//            this->propagate(parent);
//    }

};





#endif //TRACK_RACING_TREESUM_HPP
