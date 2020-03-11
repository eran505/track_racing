//
// Created by ERANHER on 11.3.2020.
//

/**
 *  This is the proportional sampling variant of the prioritized experience replay as described
    in https://arxiv.org/pdf/1511.05952.pdf.
 * */
#ifndef TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
#define TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP

#include "SumTree.hpp"

typedef experienceTuple* dataTuple;

class prioritizedExperienceReplay{
    unsigned int powerOf2Size;

    const int MAX_MEMORY_CAPACITY = 200000;

    SumTree* opSumTree;
    float alpha;
    float epsilon;
    float maximalPriority;
    bool allowDuplicatesInBatchSampling;
    default_random_engine eng{static_cast<long unsigned int>(/*time(0)*/324134)}; //Will be used to obtain a seed for the random number engine


public:

    explicit prioritizedExperienceReplay(unsigned int size,float _alpha=0.6, float _epslion=1e-6,
                                         float maximal_priority=1.0, bool _allowDuplicatesInBatchSampling=true): powerOf2Size(1),
                                                                                                                 alpha(_alpha), maximalPriority(maximal_priority)
            , epsilon(_epslion), allowDuplicatesInBatchSampling(_allowDuplicatesInBatchSampling){
        while(size > powerOf2Size) powerOf2Size *=2;
        this->opSumTree = new SumTree(powerOf2Size, operationTree::addTree);

    }

    void updatePriority(unsigned int leafIdx, float error){

        if (error<0)
            throw std::invalid_argument( "The priorities must be non-negative values" );
        auto priority = (error + this->epsilon);
        auto newPriority = powf(priority,this->alpha);
        this->opSumTree->update(leafIdx,newPriority);
    }

    void add(float error, experienceTuple* sample){
        auto p =  powf((error + epsilon),this->alpha);
        this->opSumTree->add(p,sample);
    }


    void updatePriorities(vector<unsigned int> leafIdxVec,vector<float> errorVec)
    {
        for (size_t i = 0; i < leafIdxVec.size(); ++i)
            this->updatePriority(leafIdxVec[i],errorVec[i]);
    }
    /**
     Sample a batch of transitions form the replay buffer. If the requested size is larger than the number
     of samples available in the replay buffer then the batch will return empty.
     :param size: the size of the batch to sample
     :return: a batch (list) of selected transitions from the replay buffer
    **/
    vector<dataTuple> sample(unsigned int batchSize)
    {
        vector<dataTuple> batch(batchSize);

        auto segment = this->opSumTree->total()/float(batchSize);
        for (int i = 0; i < batchSize; ++i) {
            auto a = segment * float(i);
            auto b = segment * (float((i + 1)));
            std::uniform_real_distribution<> dis(a, b);
            auto  s =  dis(eng);
            auto tupIndexes = this->opSumTree->getElementByPartialSum(s);
            auto idxTreeError = std::get<0>(tupIndexes);
            auto idxData = std::get<1>(tupIndexes);
            batch.push_back(this->opSumTree->getData(idxData));
        }
        return batch;

    }

};

#endif //TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
