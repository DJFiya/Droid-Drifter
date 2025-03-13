#include "stdafx.h"
#include "AIPlayer.h"
#include "AINetwork.h"

AIPlayer::AIPlayer(Player* player, TileMap* tileMap) {
    this->player = player;
    this->tileMap = tileMap;
    this->aiNetwork = new AINetwork(8, 10);
    srand(time(0));
    this->id = rand();
}

void AIPlayer::update() {
    sf::Vector2f position = player->getPosition();
    sf::Vector2f velocity = player->getVelocity();

    int tileX = int(position.x) / int(this->tileMap->getTileSize());
    int tileY = int(position.y) / int(this->tileMap->getTileSize());

    int tileKey = tileX * 10000 + tileY/4; // Create unique key for this tile position
    visitedTiles[tileKey] = visitedTiles[tileKey] + 1;
    recentTiles[tileKey] = totalFrames;

    // Get surrounding tiles
    Tile* belowTile = this->tileMap->getTile(tileX, tileY + 1);
    Tile* frontTile = this->tileMap->getTile(tileX + (velocity.x > 0 ? 1 : -1), tileY);
    Tile* aboveTile = this->tileMap->getTile(tileX, tileY - 1);
    Tile* behindTile = this->tileMap->getTile(tileX - (velocity.x > 0 ? 1 : -1), tileY); // New

    // Function to encode tile state
    auto getTileValue = [](Tile* tile) -> float {
        if (tile == nullptr) return 0.0;  // Empty
        else if (tile->getDamaging()) return 2.0;  // Damaging
        else if (tile->isWin()) return 3.0;  // Win
        else return 1.0;
        };

    // Get normalized tile states
    double belowState = static_cast<double> (getTileValue(belowTile) * 2.0 / 3.0 - 1.0);
    double frontState = static_cast<double> (getTileValue(frontTile) * 2.0 / 3.0 - 1.0);
    double aboveState = static_cast<double> (getTileValue(aboveTile) * 2.0 / 3.0 - 1.0);
    double behindState = static_cast<double> (getTileValue(behindTile) * 2.0 / 3.0 - 1.0);

    // Normalize inputs and include tile states
    torch::Tensor state = torch::tensor({
        static_cast<double>(position.x / 3200.0 - 1.0),
        static_cast<double>(position.y / 320.0 - 1.0),
        static_cast<double>(velocity.x / 10.0),
        static_cast<double>(velocity.y / 25.0),
        belowState, frontState, aboveState, behindState
        });

    float epsilon = 0.1f; 
    bool actionJump;
    int actionMove;

    if (static_cast<float>(std::rand()) / RAND_MAX < epsilon) {
        // Choose random actions
        actionJump = (std::rand() % 2 == 0);  // 50% chance to jump
        int moveChoice = std::rand() % 3;      // 0=left, 1=right, 2=none
        actionMove = (moveChoice == 0) ? -1 : (moveChoice == 1) ? 1 : 0;
    }
    else {
        // Use network for actions
        auto [networkJump, networkMove] = this->aiNetwork->predictAction(state);
        actionJump = networkJump;
        actionMove = networkMove;
    }

    if (actionJump) {
        jump();
    }
    if (actionMove == -1) {
        move(-1);
    }
    else if (actionMove == 1) {
        move(1);
    }

    if (position.x > farthestPositionX) {
        farthestPositionX = position.x;
        framesSinceFarthestPosition = 0;
    }
    else {
        framesSinceFarthestPosition++;
    }

    std::deque<float> positionHistory;
    const int historySize = 15;
    int stuckCounter = 0;

    positionHistory.push_back(position.x);
    if (positionHistory.size() > historySize) {
        positionHistory.pop_front();
    }
    float reward = 0.0f;

    if (frontTile && frontTile->isWin() || belowTile && belowTile->isWin()) {
        reward += 25.0f;  
    }
    else if (belowTile && belowTile->getDamaging()) {
        reward -= 1.5f;
    }
    else {
        if (position.x > previousX + 20) {
            reward += 1.0f;

            if (position.x - lastSignificantX > SIGNIFICANT_MOVEMENT) {
                reward += 5.0f;
                lastSignificantX = position.x;
            }
        }
        else if (position.x + 20 < previousX) {
            reward += 0.3f;
        }
    }
    for (auto it = recentTiles.begin(); it != recentTiles.end(); ) {
        if (totalFrames - it->second > RECENT_TILE_MEMORY) {
            it = recentTiles.erase(it);
        }
        else {
            ++it;
        }
    }

    int explorationRadius = 3; 
    int uniqueTilesInRecentMemory = recentTiles.size();
    int totalVisitsInLocalArea = 0;
    int uniqueTilesInLocalArea = 0;
    std::set<int> localTiles;

    for (int x = tileX - explorationRadius; x <= tileX + explorationRadius; x++) {
        for (int y = tileY - explorationRadius; y <= tileY + explorationRadius; y++) {
            int checkKey = x * 10000 + y;
            if (visitedTiles.find(checkKey) != visitedTiles.end()) {
                totalVisitsInLocalArea += visitedTiles[checkKey];
                localTiles.insert(checkKey);
            }
        }
    }
    uniqueTilesInLocalArea = localTiles.size();

    float averageVisitsPerTile = (uniqueTilesInLocalArea > 0) ?
        (float)totalVisitsInLocalArea / uniqueTilesInLocalArea : 0;

    if (averageVisitsPerTile > 3.0f && uniqueTilesInRecentMemory < 8) {
        smallAreaFrames++;
        float localPenalty = 0.7f * std::min(smallAreaFrames / 10, 10);
        reward -= localPenalty;
    }
    else if (uniqueTilesInRecentMemory >= 8) {
        smallAreaFrames = std::max(0, smallAreaFrames - 2);
    }
    else {
        smallAreaFrames = std::max(0, smallAreaFrames - 1);
    }

    int currentTileVisits = visitedTiles[tileKey];
    if (currentTileVisits > 5) {
        // Increasing penalty for repeatedly visiting the same tile
        reward -= 0.2f * std::min(currentTileVisits - 5, 10);
    }

    if (std::abs(position.x - previousX) < 0.1f) {
        reward -= 0.2f * std::min(stuckCounter, 10);
        stuckCounter++;
    }
    else {
        stuckCounter = std::max(0, stuckCounter - 1); 
    }

    if (positionHistory.size() == historySize) {
        float minPos = *std::min_element(positionHistory.begin(), positionHistory.end());
        float maxPos = *std::max_element(positionHistory.begin(), positionHistory.end());
        float range = maxPos - minPos;

        if (range < 1.0f && stuckCounter > 5) {
            reward -= 0.8f;  
        }
    }

    int currentChunk = static_cast<int>(position.x) / CHUNK_SIZE;
    if (exploredChunks.find(currentChunk) == exploredChunks.end()) {
        reward += 8.0f;
        exploredChunks.insert(currentChunk);
    }

    if (framesSinceFarthestPosition > PROGRESS_PATIENCE) {
        float progressPenalty = 1.5 * (1.0f + (framesSinceFarthestPosition - PROGRESS_PATIENCE) / 50.0f);
        reward -= progressPenalty;
    }


    previousX = position.x;
    totalFrames++;

    lastReward = reward;
    aiNetwork->updateReward(reward);

}

void AIPlayer::move(int r) {
    player->move(r, 0);
}

void AIPlayer::jump() {
    player->jump();
}

void AIPlayer::saveModel(const std::string& filename){
    aiNetwork->saveModel(filename);
}


