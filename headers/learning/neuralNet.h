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
    public:

        neuralNet(){
             fc1 = register_module("fc1", torch::nn::Linear(10, 64));
             fc2 = register_module("fc2", torch::nn::Linear(64, 32));
             fc3 = register_module("fc3", torch::nn::Linear(32, 23));
            batchSizeEntries=30;
        }
        torch::Tensor forward(torch::Tensor x);
        ~neuralNet() override = default;;
        void start();
        void updateNet();
        double getQvalue(State *pState, Point *pPoint);
        Tensor predictValue(vector<float> *state);
        double getQvalueMAX(State *pState);
        void calcQtraget(const ReplayBuffer *buffer,int index);

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
    Tensor neuralNet::predictValue(vector<float> *state)
    {

        ArrayRef<float> xx = *state;
        auto Sstate = torch::tensor(xx);

        this->eval(); // puts network in evaluation mode

        torch::NoGradGuard noGrad;
        auto action_values = this->forward(Sstate);
        //auto vecRes = action_values.data<float>();
        auto sizeVec0 = action_values.size(0);

        this->train(); //puts network back in training mode

        return action_values;
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
            buffer->nNextStates[entryIndx];
        }
    }
    void neuralNet::calcQtraget(const ReplayBuffer *buffer,int index) {
        vector<float> nNextStateExpectedReward;
        
        torch::Tensor Qtarget;
        {
            // This scope calc the expected value of the transition
            for (const auto nNextItem :  buffer->nNextStates[index]){
                ArrayRef<float> xx = *nNextItem;
                auto Sstate = torch::tensor(xx);
                torch::NoGradGuard noGrad;// This only takes effect within the scope
                Qtarget = this->forward(Sstate); // Does not accumulate gradient
                nNextStateExpectedReward.push_back(Qtarget.max(0))
            }
        }
    }
    void neuralNet::start() {
        cout<<"start Function"<<endl;
        // Create a multi-threaded data loader for the MNIST dataset.
    auto data_loader = torch::data::make_data_loader(
            torch::data::datasets::MNIST("./data").map(
                    torch::data::transforms::Stack<>()),
            /*batch_size=*/64);

    // Instantiate an SGD optimization algorithm to update our Net's parameters.
    torch::optim::SGD optimizer(this->parameters(), /*lr=*/0.01);

    for (size_t epoch = 1; epoch <= 10; ++epoch) {
        size_t batch_index = 0;
        // Iterate the data loader to yield batches from the dataset.
        for (auto& batch : *data_loader) {
            // Reset gradients.
            optimizer.zero_grad();
            // Execute the model on the input data.
            // compute the next state value without no-grand
            //        with torch.no_grad():
            //            Q_targets = self.compute_q_targets(next_states, rewards, dones)
            torch::Tensor Qtarget;
            {
                // This scope calc the expected value of the transition
                torch::NoGradGuard noGrad;// This only takes effect within the scope
                Qtarget = this->forward(reinterpret_cast<Tensor &&>(batch)); // Does not accumulate gradient
            }
            torch::Tensor qExoected = this->forward(reinterpret_cast<Tensor &&>(batch));
            // Compute a loss value to judge the prediction of our model.
            torch::Tensor loss = torch::mse_loss(qExoected,Qtarget);
            // Compute gradients of the loss w.r.t. the parameters of our model.
            loss.backward();

            //similar decreasing the learning rate only for big gradients to perform small updates all the time.
            //cliped ref https://discuss.pytorch.org/t/clip-gradients-norm-in-libtorch/39524/2

            // Update the parameters based on the calculated gradients.
            optimizer.step();
            if (++batch_index % 100 == 0) {
                std::cout << "Epoch: " << epoch << " | Batch: " << batch_index
                          << " | Loss: " << loss.item<float>() << std::endl;

                // Serialize your model periodically as a checkpoint.
                //torch::save(net, "net.pt")
            }
        }
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
