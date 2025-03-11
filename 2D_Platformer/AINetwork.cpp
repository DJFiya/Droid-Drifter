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

    // Train only if we have enough experiences
    if (state_memory.size() < 32) return;

    optimizer.zero_grad();

    // Convert vectors to tensors
    torch::Tensor states = torch::stack(state_memory);
    torch::Tensor actions = torch::stack(action_memory);
    torch::Tensor rewards = torch::tensor(reward_memory);

    // Forward pass
    torch::Tensor x = forward(states);
    torch::Tensor jump_pred = torch::sigmoid(fc3_jump->forward(x));
    torch::Tensor move_pred = torch::softmax(fc3_move->forward(x), 1);

    // Compute loss
    torch::Tensor jump_loss = torch::binary_cross_entropy(jump_pred, actions.index({ torch::indexing::Slice(), 0 }));
    torch::Tensor move_loss = torch::cross_entropy_loss(move_pred, actions.index({ torch::indexing::Slice(), 1 }).to(torch::kLong));

    torch::Tensor loss = (jump_loss + move_loss) * rewards.mean(); // Reward-weighted loss

    // Backpropagation
    loss.backward();
    optimizer.step();

    // Clear memory
    state_memory.clear();
    action_memory.clear();
    reward_memory.clear();
}

void AINetwork::saveModel(const std::string& filename) {
    torch::save(shared_from_this(), filename);
}
