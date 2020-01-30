//
// Created by ERANHER on 21.1.2020.
//

#ifndef TRACK_RACING_NEURALNET_H
#define TRACK_RACING_NEURALNET_H

#include "util_game.hpp"
#include <torch/torch.h>
using namespace torch;
class neuralNet : torch::nn::Module{

    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr};

public:

    neuralNet(){
         fc1 = register_module("fc1", torch::nn::Linear(784, 64));
         fc2 = register_module("fc2", torch::nn::Linear(64, 32));
         fc3 = register_module("fc3", torch::nn::Linear(32, 1));
    }
    torch::Tensor forward(torch::Tensor x);
    ~neuralNet() override = default;;
    void start();
    void updateNet();
    double getQvalue(State *pState, Point *pPoint);

    double getQvalueMAX(State *pState);
};

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
            torch::Tensor prediction = this->forward(batch.data);
            // Compute a loss value to judge the prediction of our model.
            torch::Tensor loss = torch::nll_loss(prediction, batch.target);
            // Compute gradients of the loss w.r.t. the parameters of our model.
            loss.backward();
            // Update the parameters based on the calculated gradients.
            optimizer.step();
        }
    }
}

torch::Tensor neuralNet::forward(torch::Tensor x) {
    // Use one of many tensor manipulation functions.
    x = torch::relu(fc1->forward(x.reshape({x.size(0), 784})));
    x = torch::dropout(x, /*p=*/0.5, /*train=*/is_training());
    x = torch::relu(fc2->forward(x));
    x = torch::log_softmax(fc3->forward(x), /*dim=*/1);
    return x;
}

double neuralNet::getQvalue(State *pState, Point *pPoint) {
    return 0;
}

double neuralNet::getQvalueMAX(State *pState) {
    return 0;
}



#endif //TRACK_RACING_NEURALNET_H
