#pragma once

class AINetwork : public torch::nn::Module {

private:
    const int BATCH_SIZE = 128;
    const int MEMORY_SIZE = 1024;  
    const size_t buffer_capacity = 1000;
    std::vector<torch::Tensor> state_memory;
    std::vector<torch::Tensor> action_memory;
    std::vector<float> reward_memory;
    torch::optim::Adam optimizer{ this->parameters(), torch::optim::AdamOptions(0.001) };

    torch::nn::Linear fc1{ nullptr }, fc2{ nullptr }, fc3_jump{ nullptr }, fc3_move{ nullptr };
public:
    AINetwork(int input_size, int hidden_size);

    torch::Tensor forward(torch::Tensor x);
    std::pair<bool, int> predictAction(torch::Tensor state);
    void updateReward(float reward);
    void saveModel(const std::string& filename);
    void loadModel(const std::string& filename);

};
