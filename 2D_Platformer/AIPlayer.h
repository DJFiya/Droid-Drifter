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
    std::deque<float> positionHistory;
    int stuckCounter = 0;
    float highestRewardPosition = 0.0f;
    float positionProgressThreshold = 50.0f; 
    float cumulativeDistance = 0.0f;
    int consecutiveInactiveFrames = 0;
    int inactiveAreaCounter = 0;
    float lastSignificantX = 0.0f;
    const float SIGNIFICANT_MOVEMENT = 50.0f;
    std::map<int, int> visitedTiles; 
    int smallAreaFrames = 0;
    std::map<int, int> recentTiles; 
    const int RECENT_TILE_MEMORY = 60;
    int totalFrames = 0;

    int id;

public:
    inline int getId() { return id; };

    AIPlayer(Player* player, TileMap* tileMap);
    void update();
    void move(int r);
    void jump();
    float getLastReward() const { return lastReward; }
    void saveModel(const std::string& filename);
};

