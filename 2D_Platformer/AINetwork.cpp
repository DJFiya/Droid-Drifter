#include "stdafx.h"
#include "AINetwork.h"

AINetwork::AINetwork(int input_size, int hidden_size) {
    fc1 = register_module("fc1", torch::nn::Linear(input_size, hidden_size));
    fc2 = register_module("fc2", torch::nn::Linear(hidden_size, hidden_size));

    fc3_jump = register_module("fc3_jump", torch::nn::Linear(hidden_size, 1));   // Single neuron for jump (0 or 1)
    fc3_move = register_module("fc3_move", torch::nn::Linear(hidden_size, 3));   // 3 neurons for left, stay, right

    torch::nn::init::xavier_uniform_(fc1->weight);
    torch::nn::init::xavier_uniform_(fc2->weight);
    torch::nn::init::xavier_uniform_(fc3_jump->weight);
    torch::nn::init::xavier_uniform_(fc3_move->weight);

}

torch::Tensor AINetwork::forward(torch::Tensor x) {
    x = torch::relu(fc1->forward(x));
    x = torch::relu(fc2->forward(x));
    return x;
}

std::pair<bool, int> AINetwork::predictAction(torch::Tensor state) {
    torch::Tensor x = forward(state);

    torch::Tensor jump_output = torch::tanh(fc3_jump->forward(x)); 
    bool shouldJump = jump_output.item<float>() > 0.0; 

    torch::Tensor move_output = torch::softmax(fc3_move->forward(x).unsqueeze(0), 1);
    int moveDirection = move_output.argmax().item<int>();

    // Store experience
    state_memory.push_back(state);
    action_memory.push_back(torch::tensor({ shouldJump ? 1.0f : 0.0f, (float)moveDirection }, torch::dtype(torch::kFloat32)));

    return { shouldJump, moveDirection };
}

void AINetwork::updateReward(float reward) {
    reward_memory.push_back(reward);

    if (state_memory.size() < 32) return;

    optimizer.zero_grad();

    torch::Tensor states = torch::stack(state_memory);
    torch::Tensor actions = torch::stack(action_memory);
    torch::Tensor rewards = torch::tensor(reward_memory);

    torch::Tensor normalized_rewards = rewards;
    if (rewards.size(0) > 1) { 
        normalized_rewards = (rewards - rewards.mean()) / (rewards.std() + 1e-5);
    }

    torch::Tensor x = forward(states);
    torch::Tensor jump_pred = torch::sigmoid(fc3_jump->forward(x));
    torch::Tensor move_pred = torch::softmax(fc3_move->forward(x), 1);

    torch::Tensor jump_loss = torch::binary_cross_entropy(jump_pred, actions.index({ torch::indexing::Slice(), 0 }));
    torch::Tensor move_loss = torch::cross_entropy_loss(move_pred, actions.index({ torch::indexing::Slice(), 1 }).to(torch::kLong));
    torch::Tensor loss = (jump_loss + move_loss) * rewards.mean(); 

    loss.backward();
    optimizer.step();

    const size_t keep_size = 8; 
    if (state_memory.size() > keep_size) {
        state_memory.erase(state_memory.begin(), state_memory.begin() + (state_memory.size() - keep_size));
        action_memory.erase(action_memory.begin(), action_memory.begin() + (action_memory.size() - keep_size));
        reward_memory.erase(reward_memory.begin(), reward_memory.begin() + (reward_memory.size() - keep_size));
    }

    state_memory.clear();
    action_memory.clear();
    reward_memory.clear();
}

void AINetwork::saveModel(const std::string& filename) {
    try {
        torch::serialize::OutputArchive archive;
        this->save(archive);
        archive.save_to(filename);
        std::cout << "Successfully saved model state to: " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error saving model: " << e.what() << std::endl;
    }
}

void AINetwork::loadModel(const std::string& filename) {
    try {
        torch::serialize::InputArchive archive;
        archive.load_from(filename);
        this->load(archive);
        std::cout << "Successfully loaded model state from: " << filename << std::endl;
    }
    catch (const std::exception& e) {
        std::cerr << "Error loading model: " << e.what() << std::endl;
    }
}
