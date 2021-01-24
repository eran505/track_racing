//
// Created by eranhe on 1/10/21.
//

#ifndef TRACK_RACING_DEEPAGENT_HPP
#define TRACK_RACING_DEEPAGENT_HPP
#include "Networks/DQN.hpp"
#include <torch/script.h>
#include <headers/Policy/Policy.hpp>
#include <utility>
#include "util_game.hpp"
#include "headers/learning/ReplayBuffer/prioritizedExperienceReplay.hpp"
#include "headers/learning/ReplayBuffer/Featuerzer.hpp"
#include "Networks/FF_net.hpp"
#include "ReplayBuffer/ReplyBuff.hpp"
class deepAgent{
    State::agentEnum id_agent = State::agentEnum::D;
    weightedPositionVector initialPosition;
    PriorBuffer<elementItem> buffer;
    u_int32_t counter_idx=0;
    // save the last step for the replay buffer (S,A)
    int last_action{};
    torch::Tensor last_state_stream;
    u_int64_t hash_last=0;

    unordered_map<int,Point> hashActionMap;
    //normliztion vector
    Featuerzer _featuerzer;

    //reward
    Rewards rewards_object = Rewards::getRewards();

    //net
    FF_net cur_net;
    FF_net target_net;
    std::unique_ptr<torch::optim::Adam> dqn_optimizer= nullptr;
    double epsilon_start = 0.6;
    double epsilon_final = 0.1;
    int64_t epsilon_decay = 30000;
    double epsilon=epsilon_start;
    int64_t batch_size = 4;
    float gamma = rewards_object.discountF;
    static constexpr int num_of_action=5;
    //randomzier
    std::unique_ptr<Randomizer> rander= nullptr;
    int max_speed=1;
public:

    deepAgent(std::vector<weightedPosition> &&start_positions,vector<int> normalization,const Point& Gird_size,vector<Point> l_goals,int seed
              ,u_int32_t buffer_size=50)
    :initialPosition(start_positions),buffer(buffer_size,1.0,1e-6,true),
    _featuerzer(std::move(normalization),Gird_size,std::move(l_goals)),
     cur_net(_featuerzer.get_input_size()),
     target_net(_featuerzer.get_input_size()),
     rander(std::make_unique<Randomizer>(seed))
     {
        cout<<"building\n";
        dqn_optimizer=std::make_unique<torch::optim::Adam>(cur_net.parameters(),
                                                           torch::optim::AdamOptions(0.0001));
    }

    void do_action(State *s){


         auto [tensor,hash_l] = _featuerzer.get_featers(s->to_mini_vector());
        last_state_stream=tensor;
        hash_last=hash_l;

        if(rander->get_double()>this->epsilon) {
            torch::Tensor q_values = cur_net.forward(last_state_stream);
            last_action = q_values.argmax(0).item().toInt();
        }
        else{
            last_action = int( (rander->get_double() - 0.001) * num_of_action);
        }

        s->applyAction(this->id_agent,_featuerzer.int_to_point_action_2d(last_action),this->max_speed);

    }


    void do_action_eval(State *s){
        auto [tensor,hash_l] = _featuerzer.get_featers(s->to_mini_vector());
        torch::Tensor q_values = cur_net.forward(tensor);
        s->applyAction(this->id_agent,_featuerzer.int_to_point_action_2d(q_values.argmax(0).item().toInt()),
                       this->max_speed);
    }



    const weightedPosition& get_pos(double number)
    {

        auto size = initialPosition.size();
        if (size==1)
            return initialPosition.front();
        auto idx = int(number*(size+0.99));
        return initialPosition[idx];

    }
    void reset()
    {

    }
    State::agentEnum get_id()
    {
        return id_agent;
    }

    void push_batch_to_buffer(const std::vector<unsigned int> &idx_vec,
                              const vector<float>& td_err)
    {
        buffer.updatePriorities_batch(idx_vec,td_err);
    }

    torch::Tensor compute_td_loss(u_int32_t k_batch_size)
    {

        auto sample_batch = buffer.sample_queue(k_batch_size);
        //cout<<"[buffer] size: "<<buffer.size_buffer()<<endl;
        std::vector<torch::Tensor> states;
        std::vector<torch::Tensor> new_states;
        std::vector<torch::Tensor> actions;
        std::vector<torch::Tensor> rewards;
        std::vector<torch::Tensor> dones;
        for (const auto &i : sample_batch){
            elementItem objective = buffer.get_data_by_index(i);
            states.push_back(objective.exp._state);
            new_states.push_back(std::move(objective.exp._next_state));
            actions.push_back(std::move(objective.exp._action));
            dones.push_back(std::move(objective.exp._done));
            rewards.push_back(std::move(objective.exp._reward));
        }


        auto states_tensor = torch::cat(states, 0).view({k_batch_size,22}).requires_grad_();
        auto new_states_tensor = torch::cat(new_states, 0).view({k_batch_size,22}).requires_grad_(false);
        auto actions_tensor = torch::cat(actions, 0).view({k_batch_size,1}).requires_grad_(false);
        auto rewards_tensor = torch::cat(rewards, 0).view({k_batch_size,1}).requires_grad_(false);
        auto dones_tensor = torch::cat(dones, 0).view({k_batch_size,1}).requires_grad_(false);


//        cout<<"S:"<<states_tensor<<endl;
//        cout<<"A:"<<actions_tensor<<endl;
//        cout<<"R:"<<rewards_tensor<<endl;
//        cout<<"Done:"<<dones_tensor<<endl;
//        cout<<"S':"<<new_states_tensor<<endl;


        torch::Tensor q_values = cur_net.forward(states_tensor);
        torch::Tensor next_target_q_values = cur_net.forward(new_states_tensor);

        torch::Tensor q_value = q_values.gather(1,actions_tensor);
        torch::Tensor next_q_value  = next_target_q_values.max_values(1).unsqueeze(1);


        //cout<<"q_values: "<<q_values<<endl;
        //cout<<"actions_tensor: "<<actions_tensor<<endl;


        torch::Tensor td_target = rewards_tensor + (gamma*next_q_value.detach()*(1-dones_tensor));

        //cout<<"td_error: "<<td_target<<endl;
        //cout<<"q_value: "<<q_value<<endl;

        torch::Tensor loss = torch::smooth_l1_loss(q_value, td_target);

        torch::Tensor td_err_ten = (-next_q_value.flatten()+td_target.flatten()).abs();


        //cout<<"requires_grad:\t"<<loss.requires_grad()<<endl;

        dqn_optimizer->zero_grad();
        loss.backward();
        dqn_optimizer->step();

        //cout<<"loss: "<<loss.item().toDouble()<<endl;

        std::vector<float> loss_vec(td_err_ten.data_ptr<float>(), td_err_ten.data_ptr<float>() + td_err_ten.numel());

        push_batch_to_buffer(sample_batch,loss_vec);
       //cout<<"td_error: "<<loss_vec.front()<<endl;

//        torch::NoGradGuard guard;
//        auto dico = cur_net.named_parameters();
//        for (auto &iItem : dico) {
//            cout<<iItem.pair().first<<endl;
//            cout<<iItem.pair().second<<endl;
//            cout<<"------------------------------"<<endl;
//        }
//        for (auto& item :  cur_net.parameters())
//            cout<<item.data()<<endl;

        return loss;
    }

    template<typename T=std::vector<int>>
    void learn(const State *s,bool done,float reward)
    {
        auto options = torch::TensorOptions().dtype(torch::kFloat32);

        torch::Tensor reward_tensor = torch::tensor({reward}, {torch::kFloat32});
        torch::Tensor done_tensor = torch::tensor({done}, {torch::kInt64});
        torch::Tensor action_tensor_new = torch::tensor({last_action}, {torch::kInt64});

        torch::Tensor state_tensor = last_state_stream.clone();
        auto [state_next_tensor,h_next_state] = _featuerzer.get_featers(std::forward<T>(s->to_mini_vector()));


        // make object exp
        auto expItem = elementItem(std::move(last_state_stream),hash_last,
                    std::move(state_next_tensor),h_next_state,
                    std::move(action_tensor_new),std::move(reward_tensor),std::move(done_tensor));

        // cal the td-err

       // auto td_error_socre = buffer.biggest_error;
        // insert to buffer
        if (reward>0) // TODO: if positive reward makes effect
            buffer.push(10.0,std::move(expItem));
        else
            buffer.push(0.7,std::move(expItem));
        if (counter_idx>=5)
        {
            compute_td_loss(2);
        }

        if(counter_idx%1==0)
            softUpdate(this->cur_net,this->target_net);

        epsilon_by_step(counter_idx++);

    }

    void loadstatedict(torch::nn::Module& model,
                                torch::nn::Module& target_model) {

        torch::autograd::GradMode::set_enabled(false);  // make parameters copying possible
        //torch::NoGradGuard guard;
        auto new_params = target_model.named_parameters(); // implement this
        auto params = model.named_parameters(true /*recurse*/);
        auto buffers = model.named_buffers(true /*recurse*/);
        for (auto& val : new_params) {
            auto name = val.key();
            auto* t = params.find(name);
            if (t != nullptr) {
                t->copy_(val.value());
            } else {
                t = buffers.find(name);
                if (t != nullptr) {
                    t->copy_(val.value());
                }
            }
        }

        torch::autograd::GradMode::set_enabled(true);
    }
    void epsilon_by_step(int64_t step){
        epsilon =  epsilon_final + (epsilon_start - epsilon_final) * exp(-1. * step / epsilon_decay);
        //if(step%1000==0)
            //cout<<"--> epsilon:"<<epsilon<<endl;
    }

    static void softUpdate(const torch::nn::Module& net,torch::nn::Module& target,double soft=0.1){//0.005

        torch::autograd::GradMode::set_enabled(false);
        auto new_params = net.named_parameters(); // implement this
        auto params = target.named_parameters(true /*recurse*/);
        auto buffers = target.named_buffers(true /*recurse*/);
        for (auto& val : new_params) {
            auto name = val.key();
            auto* t = params.find(name);
            if (t != nullptr) {
                //t->copy_(val.value()*soft);
                t->mul_(1-soft);
                t->add_(val.value()*soft);
            } else {
                t = buffers.find(name);
                if (t != nullptr) {
                    //t->copy_(val.value()*soft);
                    t->mul_(1-soft);
                    t->add_(val.value()*soft);
                }
            }
        }
        torch::autograd::GradMode::set_enabled(true);
    }

};



#endif //TRACK_RACING_DEEPAGENT_HPP
