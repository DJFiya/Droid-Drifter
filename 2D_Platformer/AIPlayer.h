#pragma once
#include "Player.h"
#include "TileMap.h"
#include "AINetwork.h"

class AIPlayer {
private:
    Player* player;
    TileMap* tileMap;
    AINetwork* aiNetwork;
    float previousX;
    float lastReward = 0.0f;
    float farthestPositionX = 0.0f;
    int framesSinceFarthestPosition = 0;
    const int PROGRESS_PATIENCE = 300;
    std::set<int> exploredChunks;
    const int CHUNK_SIZE = 320;

public:
    AIPlayer(Player* player, TileMap* tileMap);
    void update();
    void move(int r);
    void jump();
    float getLastReward() const { return lastReward; }
    void saveModel(const std::string& filename);
};

