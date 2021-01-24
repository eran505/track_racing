//
// Created by ERANHER on 11.3.2020.
//

/**
 *  This is the proportional sampling variant of the prioritized experience replay as described
    in https://arxiv.org/pdf/1511.05952.pdf.
 * */
#ifndef TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
#define TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
#include <torch/torch.h>

#include "TreeSum.hpp"

template <typename T>
class PriorBuffer{

    unsigned int powerOf2Size;


    short ctr=0;
    bool ready;
    std::unique_ptr< SumTree<T>> opSumTree= nullptr;
    double alpha;
    double epsilon;
    bool allowDuplicatesInBatchSampling;
    std::default_random_engine generator;
    std::uniform_real_distribution<double> distribution;
    int incermantl_size=0;
public:
    vector<unsigned int> batchSampleIndex;
    vector<T> batchSampleData;
public:
    explicit PriorBuffer(unsigned int size,double _alpha=0.6, double _epslion=1e-6,
                          bool _allowDuplicatesInBatchSampling=true, int seed=1234):powerOf2Size(1),
                         alpha(_alpha)
        , epsilon(_epslion), allowDuplicatesInBatchSampling(_allowDuplicatesInBatchSampling),generator(seed){
        powerOf2Size = std::pow(2,int(log2(size))+1);
        //while(size > powerOf2Size) powerOf2Size *=2;
        this->opSumTree = std::make_unique<SumTree<T>>(powerOf2Size, operationTree::addTree);
        ready=false;
    }

    void push(double error, T sample){
        this->ctr++;
        this->incermantl_size++;
        auto p =  powf((error + epsilon),this->alpha);
        this->opSumTree->add(p,sample);
    }

    /**
     Sample a batch of transitions form the replay buffer. If the requested size is larger than the number
     of samples available in the replay buffer then the batch will return empty.
     :param size: the size of the batch to sample
     :return: a batch (list) of selected transitions from the replay buffer
    **/
    const std::vector<u_int>& sample_queue(unsigned int batchSize)
    {
        if(batchSize-batchSampleIndex.size()!=0) {
            batchSampleIndex = vector<unsigned int>(batchSize);
        }

        auto segment = this->opSumTree->total()/double(batchSize);
        for (int i = 0; i < batchSize; ++i) {
            auto a = segment * double(i);
            auto b = segment * (double((i + 1)));
            a=0;
            b=this->opSumTree->total();
            auto s = distribution(generator) * (b-a) + a;
            auto tupIndexes = this->opSumTree->getElementByPartialSum(s);
            auto idxTreeError = std::get<0>(tupIndexes);
            auto idxData = std::get<1>(tupIndexes);
           // cout<<idxData<<":"<<this->opSumTree->tree[idxTreeError]<<" | ";
            batchSampleIndex[i]=idxData;
        }
      //  cout<<endl;
        return batchSampleIndex;
    }

    T get_data_by_index(u_int64_t idx)
    {
        return opSumTree->dataTree[idx];
    }

    void updatePriority(unsigned int leafIdx, double error){

        assert(error>=0);
        auto priority = (error + this->epsilon);
        auto newPriority = powf(priority,this->alpha);
        this->opSumTree->update(leafIdx,newPriority);
    }

    void updatePriorities_batch(const vector<unsigned int> &leafIdxVec,const vector<float> &errorVec)
    {
        for (size_t i = 0; i < leafIdxVec.size(); ++i)
            this->updatePriority(leafIdxVec[i],errorVec[i]);
    }

};





























template<class T>
struct priority_queue_uniform : std::vector<T,std::vector<T>> {
    T&& move_front() {
        return std::move(this->c.front());
    }
    const T& fist_val() {
        return this->c.front();
    }

};


template<class T>
struct priority_queue_my : std::priority_queue<T,std::vector<T>> {
    T&& move_front() {
        return std::move(this->c.front());
    }
    const T& fist_val() {
        return this->c.front();
    }

};


struct element{
    float_t td_error;
    int64_t index;
    std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> transition;

    element(float_t error, int64_t ind,
            std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> trans):
            td_error(error),
            index(ind),
            transition(trans)
    {}

    bool operator<(const struct element& other) const{
        if (td_error != other.td_error){
            return td_error < other.td_error;
        }else{
            return index < other.index;
        }
    }
};

class PrioritizedExperienceReplay{

private: int64_t capacity;
private: float_t alpha=0.9;
public: priority_queue_my<element> buffer;
public: float_t biggest_error=1;

public:
    explicit PrioritizedExperienceReplay (int64_t size);
    void push(torch::Tensor &&state,torch::Tensor &&new_state,torch::
    Tensor &&action,torch::Tensor &&done,torch::Tensor &&reward, float_t td_error, int64_t index);
    [[nodiscard]] int64_t size_buffer() const;
    std::vector<std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor>> sample_queue(int64_t batch_size);


};

PrioritizedExperienceReplay::PrioritizedExperienceReplay(int64_t size) {
    capacity = size;
}

void PrioritizedExperienceReplay::push(torch::Tensor &&state,torch::Tensor &&new_state,torch::
Tensor &&action,torch::Tensor &&done,torch::Tensor &&reward, float_t td_error, int64_t ind){
    float_t error(td_error*alpha);
    int64_t index(ind);
    std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> sample (state, new_state, action, reward, done);
    element sample_struct(error, index, sample);
    if(biggest_error<td_error)
        biggest_error=td_error;
    if (buffer.size() < capacity){
        buffer.push(sample_struct);
    }
    else {
        while (buffer.size() >= capacity) {
            buffer.pop();
        }
        buffer.push(sample_struct);
    }
}

std::vector<std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor>>
PrioritizedExperienceReplay::sample_queue(
        int64_t batch_size){
    u_int16_t ctr=0;
    std::vector<std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor>> b;
    b.reserve(batch_size);
    while (batch_size > 0 and !buffer.empty()){
        element s = buffer.move_front();
        std::tuple<torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor, torch::Tensor> sample = s.transition;
        b.emplace_back(sample);
        buffer.pop();
        if(++ctr==batch_size)
            break;
    }
    return b;
}

int64_t PrioritizedExperienceReplay::size_buffer() const{

    return buffer.size();
}


#endif //TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
