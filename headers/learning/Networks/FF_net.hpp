//
// Created by eranhe on 1/11/21.
//

#ifndef TRACK_RACING_FF_NET_HPP
#define TRACK_RACING_FF_NET_HPP
#include <torch/script.h>

//using namespace torch;

class FF_net : public torch::nn::Module {
    torch::nn::Linear linear1;
    torch::nn::Linear linear2;
    torch::nn::Linear hidden;
    torch::nn::Linear linear3;
    torch::Device _device;
public:
    explicit FF_net(int64_t num_inputs, int64_t numActions=5,u_int32_t  hidden_nodes=128):
            linear1(register_module("L1",torch::nn::Linear(num_inputs, hidden_nodes))),
            linear2(register_module("L2",torch::nn::Linear(hidden_nodes, hidden_nodes))),
            hidden(register_module("H0",torch::nn::Linear(hidden_nodes, hidden_nodes))),
            linear3(register_module("L3",torch::nn::Linear(hidden_nodes, numActions))),
            _device(torch::Device(torch::cuda::is_available() ? torch::kCUDA : torch::kCPU)){

        std::cout << (torch::cuda::is_available() ? "CUDA available. Training on GPU." : "Training on CPU.") << '\n';
        this->to(_device);

    }

    torch::Tensor forward(torch::Tensor input) {
        // Flatten the output
        input = input.to(_device);
        input = linear1->forward(input);
        input =torch::relu_(input);
        //input = hidden->forward(input);
        //input =torch::relu_(input);
        input = linear2->forward(input);
        input = torch::relu_(input);
        input = linear3->forward(input);
        return torch::clamp_(input,-1,1);
        return input;
    }


};
#endif //TRACK_RACING_FF_NET_HPP
