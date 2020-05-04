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
#include <random>
using namespace torch;
class neuralNet : torch::nn::Module{

    torch::nn::Linear fc1{nullptr}, fc2{nullptr}, fc3{nullptr}, fc4{nullptr} ;
    int batchSizeEntries;
    int ctrEp;
    float GRADIENT_CLIP;
    torch::Device* deviceI;
    torch::optim::SGD* optimizer= nullptr;
    public:

        explicit neuralNet(int featuresIn, int actionSize=27):deviceI(nullptr){
             fc1 = register_module("fc1", torch::nn::Linear(featuresIn, featuresIn));
             fc2 = register_module("fc2", torch::nn::Linear(featuresIn, featuresIn));
             fc3 = register_module("fc3", torch::nn::Linear(featuresIn, featuresIn));
             fc4 = register_module("fc4", torch::nn::Linear(featuresIn, actionSize));
             ctrEp=0;
             batchSizeEntries=1;
             setOptimizer();
             GRADIENT_CLIP=1;
             // Instantiate an SGD optimization algorithm to update our Net's parameters.
            // Device
            auto cuda_available = torch::cuda::is_available();
            deviceI = new torch::Device(cuda_available ? torch::kCUDA : torch::kCPU);
            std::cout << (cuda_available ? "CUDA available. Training on GPU." : "Training on CPU.") << '\n';
            this->to(*deviceI);

        }
        torch::Tensor forward(torch::Tensor x);
        ~neuralNet() {
            delete deviceI;
            delete optimizer;
        };
        //void updateN et();
        double getQvalue(State *pState, Point *pPoint);
        Tensor evalArgMaxQ(Tensor &state);
        double getQvalueMAX(State *pState);
        Tensor calcQtraget(const ReplayBuffer *buffer,int index);
        Tensor getActionStateValue(vector<float> *state, int actionIndx);
        void learn(const Tensor& qCurState,const Tensor& qNextState);
        void setOptimizer(){
            optimizer = new torch::optim::SGD(this->parameters(),torch::optim::SGDOptions(0.001) );
            //adam (to make thing faster)
        }
        Tensor getEvalMaxValue(Tensor &Qtarget);
        void updateNet(const ReplayBuffer *buffer);

    float get_grad_norm(int grad_norm_type);
};


    Tensor neuralNet::getActionStateValue(vector<float> *state, int actionIndx)
    {
        //auto x_train = torch::randint(0, 10, {128});
        //cout<<x_train<<endl;
        ArrayRef<float> xx = *state;
        auto Sstate = torch::tensor(xx).to(*this->deviceI);
        Sstate.requires_grad_();

        auto res = this->forward(Sstate);
        //cout<<res<<endl;
        //TODO: requires_grad =true ?
        if (actionIndx>=0)
            res = res.operator[](actionIndx);
        //TODO: can be the case when we have more than one arg_max -> choose randomly one !
        return res;
    }


    Tensor neuralNet::evalArgMaxQ(Tensor &state) // TODO: return int more efficent
    {
        //cout<<state<<endl;
        //cout<<"state:\t\t"<<state<<endl;
        this->eval(); // puts network in evaluation mode
        torch::NoGradGuard noGrad;
        auto action_values = this->forward(state);

        // for debug


        //cout<<"action_values:\t\t"<<action_values<<endl;
        //auto res = action_values.argmax(0); // get the arg-max from the tensor
      //  cout<<"ArgMax: "<<res.item().toInt()<<endl;
        this->train(); //puts network back in training mode
        //TODO: can be the case when we have more than one arg_max -> choose randomly one !

        return action_values;
    }

    void neuralNet::updateNet(const ReplayBuffer *buffer)
    {

        if (!buffer->isSufficientAmountExperience())
            return;
        //random samples
        std::unordered_set<int> entries;

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
            //cout<<"sum_of_elemsTensoer = "<<sum_of_elemsTensoer<<endl;
            //cout<<"valueCurState = "<<valueCurState<<endl;

            this->learn(valueCurState,sum_of_elemsTensoer);

        }
    }

Tensor neuralNet::getEvalMaxValue(Tensor &Qtarget) {
    torch::NoGradGuard noGrad;// This only takes effect within the scope
    auto resultsTensor = this->forward(Qtarget); // Does not accumulate gradient
    //cout<<"Q_next: "<< resultsTensor <<endl;
    auto res = resultsTensor.max(1);
    auto valuesQ = get<0>(res); // values (if you want the arg_max use <1> )
    return valuesQ;
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
//        auto target = qNextState.to(*this->deviceI);
//        auto pred = qCurState.to(*this->deviceI);
        //auto old = this->fc1->weight.clone();
        // Reset gradients.

        //for debug
        //cout<<"qNextState:\t"<<qNextState<<endl;
        //cout<<"qCurState:\t"<<qCurState<<endl;
        optimizer->zero_grad();

        // Compute a loss value to judge the prediction of our model.
        torch::Tensor loss = torch::mse_loss(qCurState,qNextState);

        //for debug
        //cout<<"loss:\t"<<loss<<endl;
        //cout<<"---------------------\n";
        //end

        // Compute gradients of the loss w.r.t. the parameters of our model.
        loss.backward();

        //similar decreasing the learning rate only for big gradients to perform small updates all the time.
        //cliped ref https://discuss.pytorch.org/t/clip-gradients-norm-in-libtorch/39524/2
        auto dico = this->named_parameters();
        for (auto &iItem : dico) {
//            cout<<iItem.pair().first<<endl;
//            cout<<iItem.pair().second<<endl;
            //cout<<"------------------------------"<<endl;
        }
        for(auto &parm:this->parameters())
        {

            //cout<<"----Before---\n\n\n";
//            cout<<parm.grad().data()<<endl;
            torch::clamp_(parm.grad(), -this->GRADIENT_CLIP, GRADIENT_CLIP);
            //cout<<"---After----\n\n\n";
//            cout<<parm.grad().data()<<endl;
        }


        // Update the parameters based on the calculated gradients.
        optimizer->step();

        //auto res = torch::eq(old,this->fc1->weight.clone());
        //cout<<res<<endl;

        if (++ctrEp % 7000 == 0) {
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
    //x = torch::dropout(x, /*p=*/0.01, /*train=*/is_training());
    x = torch::relu(fc2->forward(x));
    x =  torch::relu(fc3->forward(x));
    x = (fc4->forward(x));
    //x = torch::softmax(fc3->forward(x) ,   0);
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

float neuralNet::get_grad_norm(int grad_norm_type) {
    torch::Tensor tmp = torch::zeros({1});

    for (auto &p : this->named_parameters()) {
        auto z = p.value(); // note that z is a Tensor, same as &p : layers->parameters
        tmp += torch::pow(torch::norm(z.grad(), grad_norm_type), grad_norm_type);
    }

    auto total_norm = torch::pow(tmp, 1 / (grad_norm_type + 1e-6));
    return total_norm.item().toFloat();
}

//void neuralNet::updateNet() {
//    torch::jit::script::Module container = torch::jit::load("container.pt");
//}




#endif //TRACK_RACING_NEURALNET_H
