//
// Created by ERANHER on 21.1.2020.
//

#ifndef TRACK_RACING_NEURALNET_H
#define TRACK_RACING_NEURALNET_H

#include "util_game.hpp"
#include <torch/torch.h>
#include <iostream>
#include <memory>
#include <torch/script.h>
#include "ReplayBuffer/ReplayBuffer.hpp"
using namespace torch;
class neuralNet : torch::nn::Module{

    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};
    int batchSizeEntries;
    int ctrEp;
    torch::optim::SGD* optimizer= nullptr;
    public:

        neuralNet(){

             fc1 = register_module("fc1", torch::nn::Linear(10, 64));
             fc2 = register_module("fc2", torch::nn::Linear(64, 32));
             fc3 = register_module("fc3", torch::nn::Linear(32, 23));
             ctrEp=0;
             batchSizeEntries=1;
            setOptimizer();
            // Instantiate an SGD optimization algorithm to update our Net's parameters.

        }
        torch::Tensor forward(torch::Tensor x);
        ~neuralNet() override = default;;
        void start();
        void updateNet();
        double getQvalue(State *pState, Point *pPoint);
        int predictValue(vector<float> *state);
        double getQvalueMAX(State *pState);
        Tensor calcQtraget(const ReplayBuffer *buffer,int index);
        Tensor getActionStateValue(vector<float> *state, int indxAction);
        void learn(const Tensor& qCurState,const Tensor& qNextState);
        void setOptimizer(){
            optimizer = new torch::optim::SGD(this->parameters(), /*lr=*/0.01);
        }

        void updateNet(const ReplayBuffer *buffer);
};


//    torch::Tensor getData()
//    {
//        float data[] = { 1, 2, 3,
//                         4, 5, 6 };
//        vector<float> myData = {4,2,3,4,5,6};
//        ArrayRef<float> x = myData;
//        auto f = torch::tensor(x);
//        //torch::Tensor f = torch::from_blob(x);
//        //torch::Tensor xx =  torch::from_blob(std::data(myData), {2, 3});
//        return f;
//    }
    Tensor neuralNet::getActionStateValue(vector<float> *state, int actionIndx)
    {
        ArrayRef<float> xx = *state;
        auto Sstate = torch::tensor(xx);
        auto action_values = this->forward(Sstate);
        auto res = action_values.operator[](actionIndx);
        //TODO: can be the case when we have more than one arg_max -> choose randomly one !
        return res;

    }
    int neuralNet::predictValue(vector<float> *state)
    {
        // return the arg_max of the action with the largest expected discounted reward
        ArrayRef<float> xx = *state;
        auto Sstate = torch::tensor(xx);

        this->eval(); // puts network in evaluation mode

        torch::NoGradGuard noGrad;
        auto action_values = this->forward(Sstate);
       // cout<<action_values<<endl;
        auto res = action_values.argmax(0); // get the arg-max from the tensor
        //cout<<res<<endl;
        this->train(); //puts network back in training mode
        //TODO: can be the case when we have more than one arg_max -> choose randomly one !
        return res.item().toInt();
    }

    void neuralNet::updateNet(const ReplayBuffer *buffer)
    {

        if (!buffer->isSufficientAmountExperience())
            return;
        //random samples
        unordered_set<int> entries;

        buffer->sampleEntries(batchSizeEntries,entries);
        for (const auto entryIndx:entries)
        {
            vector<float> expReward;
            auto QMaxValues = this->calcQtraget(buffer,entryIndx);
            auto probList = buffer->pProbabilityNextStates[entryIndx];
            for (int i=0 ; i < buffer->rRewardNextStates[entryIndx]->size(); ++i)
            {
                if (buffer->rRewardNextStates[entryIndx]->operator[](i) == 0){
                    expReward.push_back(QMaxValues.operator[](i).item().toFloat()*
                    buffer->pProbabilityNextStates[entryIndx]->operator[](i));
                }
                else expReward.push_back(buffer->rRewardNextStates[entryIndx]->operator[](i)*
                buffer->pProbabilityNextStates[entryIndx]->operator[](i));
            }
            // sum the vector
            float sum_of_elems;

//            cout<<""<<endl;
            std::for_each(expReward.begin(), expReward.end(), [&] (float n) {
                sum_of_elems += n;
            });
            auto valueCurState = this->getActionStateValue(buffer->stateS[entryIndx],buffer->aAction[entryIndx]);
            //need to be tensor ->sum_of_elems
            auto sum_of_elemsTensoer = torch::tensor(sum_of_elems);
            this->learn(valueCurState,sum_of_elemsTensoer);

        }
    }
    Tensor neuralNet::calcQtraget(const ReplayBuffer *buffer,int index) {
        vector<float> nNextStateExpectedReward;
        auto x = torch::zeros({0,0});
        torch::Tensor Qtarget;
        bool isFirst=true;
        // This scope calc the expected value of the transition
        vector<at::Tensor> l ;
        for (const auto nNextItem :  buffer->nNextStates[index]) {
            ArrayRef<float> xx = *nNextItem;
            auto curTensor = torch::tensor(xx);
            l.push_back(curTensor);
        }
        Qtarget=torch::stack(l);
//        cout<<Qtarget<<endl;
//        cout<<Qtarget.sizes()<<endl;
        torch::NoGradGuard noGrad;// This only takes effect within the scope
        auto resultsTensor = this->forward(Qtarget); // Does not accumulate gradient
        auto res = resultsTensor.max(1);
        auto valuesQ = get<0>(res); // values (if you want the arg_max use <1> )
        return valuesQ;
    }
    void neuralNet::learn(const Tensor& qCurState,const Tensor& qNextState) {


        // Reset gradients.
        optimizer->zero_grad();

        // Compute a loss value to judge the prediction of our model.
        torch::Tensor loss = torch::mse_loss(qCurState,qNextState);
        // Compute gradients of the loss w.r.t. the parameters of our model.
        loss.backward();

        //similar decreasing the learning rate only for big gradients to perform small updates all the time.
        //cliped ref https://discuss.pytorch.org/t/clip-gradients-norm-in-libtorch/39524/2

        // Update the parameters based on the calculated gradients.
        optimizer->step();

        if (++ctrEp % 50 == 0) {
            cout<< " | Loss: " << loss.item<float>() << std::endl;

            // Serialize your model periodically as a checkpoint.
            //torch::save(net, "net.pt")
        }

    }





torch::Tensor neuralNet::forward(torch::Tensor x) {
    // Use one of many tensor manipulation functions.
    //x.reshape({x.size(0), 784})

    //auto input = x.reshape({x.size(0), 10});
    x = torch::relu(fc1->forward(x));
    x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
    x = torch::relu(fc2->forward(x));
    x = torch::softmax(fc3->forward(x) ,   0);
    return x;
}

double neuralNet::getQvalue(State *pState, Point *pPoint) {
    torch::NoGradGuard noGrad;
    this->eval(); // #puts network in evaluation mode

    return 0;
}

double neuralNet::getQvalueMAX(State *pState) {
    return 0;
}

void neuralNet::updateNet() {
    torch::jit::script::Module container = torch::jit::load("container.pt");
}




#endif //TRACK_RACING_NEURALNET_H
