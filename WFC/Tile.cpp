#include "Tile.h"

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

    // Calculate the total weight of the entropy options
    int total_weight = 0;
    for (int value : entropy) {
        total_weight += entropyList->at(value).weight;
    }

    // Generate a random number in the range of the total weight
    std::uniform_int_distribution<> distribution(0, total_weight - 1);
    int rng = distribution(gen);

    // Select an option based on the weighted distribution
    int accumulated_weight = 0;
    int chosen_value = entropy[0];
    for (int value : entropy) {
        accumulated_weight += entropyList->at(value).weight;
        if (rng < accumulated_weight) {
            chosen_value = value;
            break;
        }
    }

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
    if (newEntropy.size() == 1) 
    {
        entropy = newEntropy;
        return;
    }
    filterRules(right, [](const Rule& r) { return r.left; }, [](const Rule& r) { return r.right; });
    if (newEntropy.size() == 1)
    {
        entropy = newEntropy;
        return;
    }
    filterRules(down, [](const Rule& r) { return r.up; }, [](const Rule& r) { return r.down; });
    if (newEntropy.size() == 1)
    {
        entropy = newEntropy;
        return;
    }
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
