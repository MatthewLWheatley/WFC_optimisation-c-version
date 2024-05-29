#include "Tile.h"

Tile::~Tile() 
{
    
}

void Tile::SetNeighbours(Tile* _up, Tile* _right, Tile* _down, Tile* _left) {
    up = _up;
    right = _right;
    down = _down;
    left = _left;
}

void Tile::CollapseTile() 
{
    int _seed = *seed;
    std::mt19937 gen(_seed);
    std::uniform_int_distribution<> distribution(0, entropy.size() - 1);
    int rng = distribution(gen);
    int chosen_value = entropy[rng];
    entropy.clear();
    entropy.push_back(chosen_value);
    collapsed = true;
}

void Tile::Propagate() 
{
    std::vector<int> newEntropy(entropy);

    auto filterRules = [this, &newEntropy](Tile* neighbor, auto getNeighborRule, auto getCurrentRule) {
        if (neighbor == nullptr || entropyList == nullptr || neighbor->entropyList == nullptr) {
            return;
        }
        std::vector<int> tempEntropy;
        for (int key : newEntropy) {
            Rule& currentRule = (*entropyList)[key];
            bool isValid = false;
            for (int neighborKey : neighbor->entropy) {
                Rule& neighborRule = (*(neighbor->entropyList))[neighborKey];
                if (getCurrentRule(currentRule) == getNeighborRule(neighborRule)) {
                    isValid = true;
                    break;
                }
            }
            if (isValid) {
                tempEntropy.push_back(key);
            }
        }
        newEntropy = tempEntropy;
        };

    filterRules(up, [](const Rule& r) { return r.down; }, [](const Rule& r) { return r.up; });
    filterRules(right, [](const Rule& r) { return r.left; }, [](const Rule& r) { return r.right; });
    filterRules(down, [](const Rule& r) { return r.up; }, [](const Rule& r) { return r.down; });
    filterRules(left, [](const Rule& r) { return r.right; }, [](const Rule& r) { return r.left; });

    entropy = newEntropy;
}

void Tile::Reset()
{
    collapsed = false;
    entropy.clear();
    for (auto key : entropyBackup) 
    {
        entropy.push_back(key);
    }
}
