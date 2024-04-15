#include "Tile.h"

Tile::Tile() {
    // Constructor implementation
}

Tile::~Tile() {
    // Destructor implementation
}

void Tile::SetNeighbours(Tile* _up, Tile* _right, Tile* _down, Tile* _left) {
    up = _up;
    right = _right;
    down = _down;
    left = _left;
}

void Tile::CollapseTile()
{
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, entropy.size() - 1);
    int rng = distribution(gen);
    int temp = entropy[rng];
    entropy.clear();
    entropy.push_back(rng);
}

void Tile::Propogate()
{
    std::vector<std::string> viableSockets;

    // Assume up->entropy is filled correctly
    if (up != nullptr) {
        for (auto ent : up->entropy)
        {
            auto temp = (*entropyList)[ent];
            if (std::find(viableSockets.begin(), viableSockets.end(), temp.down) == viableSockets.end())
            {
                viableSockets.push_back(temp.down);
            }
        }
    }

    for (auto ent : viableSockets)
    {
        std::cout << ent << std::endl;
    }

    auto it = std::remove_if(entropy.begin(), entropy.end(), [this, &viableSockets](int id) {
        auto temp2 = (*entropyList)[id].up;
        return std::find(viableSockets.begin(), viableSockets.end(), temp2) == viableSockets.end();
        });
}

std::string ReverseString(const std::string& str) 
{
    return std::string(str.rbegin(), str.rend());
}