////
//// Created by ERANHER on 21.1.2020.
////
//
//#ifndef TRACK_RACING_NEURALNET_H
//#define TRACK_RACING_NEURALNET_H
//
//#include <torch/script.h>
//#include <torch/nn.h>
//#include <torch/optim.h>
//#include <torch/cuda.h>
//
//class NeuralNet : public torch::nn::Module{
//
//    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr}, fc4{nullptr} ;
//    int batchSizeEntries;
//    int ctrEp;
//    double GRADIENT_CLIP;
//    torch::Device* deviceI;
//    torch::optim::SGD* optimizer= nullptr;
//    public:
//
//        explicit NeuralNet(int featuresIn, int actionSize=27):deviceI(nullptr){
//             fc1 = register_module("fc1", torch::nn::Linear(featuresIn, featuresIn));
//             fc2 = register_module("fc2", torch::nn::Linear(featuresIn, featuresIn));
//             fc3 = register_module("fc3", torch::nn::Linear(featuresIn, featuresIn));
//             fc4 = register_module("fc4", torch::nn::Linear(featuresIn, actionSize));
//             ctrEp=0;
//             batchSizeEntries=1;
//             setOptimizer();
//             GRADIENT_CLIP=1;
//             // Instantiate an SGD optimization algorithm to update our Net's parameters.
//            // Device
//            auto cuda_available = torch::cuda::is_available();
//            deviceI = new torch::Device(cuda_available ? torch::kCUDA : torch::kCPU);
//            std::cout << (cuda_available ? "CUDA available. Training on GPU." : "Training on CPU.") << '\n';
//            this->to(*deviceI);
//
//        }
//        torch::Tensor forward(torch::Tensor x)
//        {
//            // Use one of many tensor manipulation functions.
//            //x.reshape({x.size(0), 784})
//
//            //auto input = x.reshape({x.size(0), 10});
//            x = torch::relu(fc1->forward(x));
//            //x = torch::dropout(x, /*p=*/0.01, /*train=*/is_training());
//            x = torch::relu(fc2->forward(x));
//            x =  torch::relu(fc3->forward(x));
//            x = (fc4->forward(x));
//            //x = torch::softmax(fc3->forward(x) ,   0);
//            return x;
//        }
//        void setOptimizer(){
//            optimizer = new torch::optim::SGD(this->parameters(),torch::optim::SGDOptions(0.001) );
//            //adam (to make thing faster)
//        }
//
//};
//
//
//
//
//
//#endif //TRACK_RACING_NEURALNET_H
