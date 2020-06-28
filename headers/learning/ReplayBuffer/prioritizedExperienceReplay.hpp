////
//// Created by ERANHER on 11.3.2020.
////
//
///**
// *  This is the proportional sampling variant of the prioritized experience replay as described
//    in https://arxiv.org/pdf/1511.05952.pdf.
// * */
//#ifndef TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
//#define TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
//
//#include "SumTree.hpp"
//
//typedef experienceTuple* dataTuple;
//
//class prioritizedExperienceReplay{
//    unsigned int powerOf2Size;
//
//    int CAPACITY_FOR_LEARNING;
//    short ctr=0;
//    bool ready;
//    SumTree* opSumTree;
//    double alpha;
//    double epsilon;
//    double maximalPriority;
//    bool allowDuplicatesInBatchSampling;
//    std::default_random_engine eng{static_cast<long unsigned int>(/*time(0)*/324134)}; //Will be used to obtain a seed for the random number engine
//
//
//public:
//    ~prioritizedExperienceReplay(){
//        delete this->opSumTree;
//    }
//
//    void numPostiveReward(){
//        short postiveRewardCounter=0;
//        short ALLpostiveRewardCounter=0;
//
//        for(auto &ptrItem:this->opSumTree->dataTree)
//            if (!(ptrItem == nullptr))
//                for (auto k: *ptrItem->ptrRewards)
//                    if (k>0)
//                        ALLpostiveRewardCounter++;
//        for(auto &ptrItem: batchSampleData)
//            if (!(ptrItem == nullptr))
//                for (auto k: *ptrItem->ptrRewards)
//                    if (k>0)
//                        postiveRewardCounter++;
//        cout<<"all: "<<ALLpostiveRewardCounter<<" batch: "<<  postiveRewardCounter<<endl;
//    }
//
//    vector<experienceTuple*> batchSampleData;
//    vector<unsigned int> batchSampleIndex;
//
//    explicit prioritizedExperienceReplay(unsigned int size,int _CAPACITY_FOR_LEARNING,double _alpha=0.6, double _epslion=1e-6,
//                                         double maximal_priority=1.0, bool _allowDuplicatesInBatchSampling=true): powerOf2Size(1),
//                                                                                                                 alpha(_alpha), maximalPriority(maximal_priority)
//            , epsilon(_epslion), allowDuplicatesInBatchSampling(_allowDuplicatesInBatchSampling){
//        while(size > powerOf2Size) powerOf2Size *=2;
//        this->opSumTree = new SumTree(powerOf2Size, operationTree::addTree);
//        ready=false;
//        CAPACITY_FOR_LEARNING=_CAPACITY_FOR_LEARNING;
//    }
//
//    void updatePriority(unsigned int leafIdx, double error){
//
//        if (error<0)
//            throw std::invalid_argument( "The priorities must be non-negative values" );
//        auto priority = (error + this->epsilon);
//        auto newPriority = powf(priority,this->alpha);
//        this->opSumTree->update(leafIdx,newPriority);
//    }
//
//    void add(double error, experienceTuple* sample){
//        this->ctr++;
//        auto p =  powf((error + epsilon),this->alpha);
//        this->opSumTree->add(p,sample);
//    }
//
//    bool readyToLearn(){
//        if (ready)
//            return true;
//        if (ctr>CAPACITY_FOR_LEARNING)
//            this->ready= true;
//        return ready;
//    }
//
//    void updatePriorities(vector<unsigned int> &leafIdxVec,vector<double> &errorVec)
//    {
//        for (size_t i = 0; i < leafIdxVec.size(); ++i)
//            this->updatePriority(leafIdxVec[i],errorVec[i]);
//    }
//    /**
//     Sample a batch of transitions form the replay buffer. If the requested size is larger than the number
//     of samples available in the replay buffer then the batch will return empty.
//     :param size: the size of the batch to sample
//     :return: a batch (list) of selected transitions from the replay buffer
//    **/
//    void sample(unsigned int batchSize)
//    {
//        this->batchSampleData.clear();
//        this->batchSampleIndex.clear();
//
//        auto segment = this->opSumTree->total()/double(batchSize);
//        for (int i = 0; i < batchSize; ++i) {
////            auto a = i%2==0? 0.0:this->opSumTree->total()/2.0;
////            auto b = i%2==0? this->opSumTree->total()/2.0:this->opSumTree->total();
////            auto a = 0;
////            auto b = this->opSumTree->total()-epsilon;
//            auto a = segment * double(i);
//            auto b = segment * (double((i + 1)));
//            std::uniform_real_distribution<> dis(a,b);
//            auto  s =  dis(eng);
//            auto tupIndexes = this->opSumTree->getElementByPartialSum(s);
//            auto idxTreeError = std::get<0>(tupIndexes);
//            auto idxData = std::get<1>(tupIndexes);
//            batchSampleIndex.push_back(idxData);
//            auto expDAta = this->opSumTree->getData(idxData);
//            assert(expDAta != nullptr);
//            batchSampleData.push_back(expDAta);
//        }
//
//    }
//
//};
//
//#endif //TRACK_RACING_PRIORITIZEDEXPERIENCEREPLAY_HPP
