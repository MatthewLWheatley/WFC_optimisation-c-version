#include "WFC.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std::chrono;

WFC::WFC(int _gridHeight, int _gridWidth, int _seed, std::string inputFile)
{
    inputFile = "input/" + inputFile;
    std::string temp = "data\\Data" + std::to_string(_seed) + ".json";
    seed = &_seed;
    width = _gridWidth;
    height = _gridHeight;
    //std::cout << "Init rules" << std::endl;
    InitRules(inputFile);
    //std::cout << "Init Grid" << std::endl;
    InitGrid();
    int count = 0;
    int lastCount = 0;
    bool completed = false;

    while (!completed)
    {
        (*seed)++;
        CollapseTile();
        count++;

        for (int x = 0; x < width; x++) 
        {
            for (int y = 0; y < height; y++) 
            {
                if (fullGridTile[std::make_pair(x, y)].collapsed) count++;
            }
        }

        if (count > width*height) 
        {
            completed = true;
        }
        if (count == lastCount) 
        {
            if (lastHighest < count) failCount = 0;
            lastHighest = count; 
            failCount++;
            std::cout << "failcount: " << failCount << std::endl;
            for (int i = 0; i < failCount; i++)
            {
                auto tile = tileStack.top();
                fullGridTile[tile].Reset();
                tileStack.pop();
            }
            for (auto tile : fullGridTile) 
            {
                if (!fullGridTile[tile.first].collapsed)
                {
                    fullGridTile[tile.first].Reset();
                }
            }
            lastCount = 0;
        }

        lastCount = count;
        count = 0;

    }

    //std::cout << "starting right" << std::endl;

    std::vector<Tile> tiles;
    for (auto tile : fullGridTile) tiles.push_back(tile.second);
    //std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;
    writeToJson(tiles, temp);
}

WFC::~WFC()
{

}

void WFC::InitRules(std::string inputFile)
{
    std::vector<std::string> _types = { "AAAA","ABBA","ACA", "ADA", "AEA", "AFA", "AGA" };
    std::vector<Rule> _rules = readCSV(inputFile);
     
    for (int id = 0; id < _rules.size(); id++)
    {
        entropyList[id] = _rules[id];
    }

    for (const auto& entropy : entropyList)
    {
        entropyKeys.push_back(entropy.first);
    }
}

std::vector<Rule> WFC::readCSV(const std::string& filename) {
    std::ifstream file(filename);
    std::string line;
    std::getline(file, line); // Read the header line and ignore it
    std::vector<Rule> _rules;

    while (std::getline(file, line)) {
        std::istringstream s(line);
        std::string field;
        Rule row;

        // Read each field from the line
        std::getline(s, row.up, ',');
        std::getline(s, row.right, ',');
        std::getline(s, row.down, ',');
        std::getline(s, row.left, ',');

        row.down = ReverseString(row.down);
        row.left = ReverseString(row.left);

        // Parse spritePosition
        std::getline(s, field, '}');
        row.spritePosition = parseIntegerList(field);

        // Parse weight
        std::getline(s, field);
        field = field.substr(1, field.size()); // Remove the braces
        row.weight = std::stoi(field);

        //// Output for demonstration
        //std::cout << "Up: " << row.up << ", Right: " << row.right << ", Down: " << row.down
        //    << ", Left: " << row.left << ", Weight: " << row.weight << ", SpritePosition: ";
        //for (int n : row.spritePosition) {
        //    std::cout << n << " ";
        //}
        //std::cout << std::endl;

        _rules.push_back(row);
    }
    
    file.close();

    return _rules;
}

std::vector<int> WFC::parseIntegerList(const std::string& str) {
    std::vector<int> result;
    std::stringstream ss(str.substr(1, str.size())); // Remove the braces
    std::string item;
    while (getline(ss, item, ',')) {
        // Trim spaces from the item2
        item.erase(remove(item.begin(), item.end(), ' '), item.end());

        // Attempt to convert to integer and catch exceptions
        try {
            result.push_back(std::stoi(item));
        }
        catch (const std::invalid_argument& e) {
            std::cerr << "Invalid argument: [" << item << "] cannot be converted to an integer." << std::endl;
        }
        catch (const std::out_of_range& e) {
            std::cerr << "Out of range: [" << item << "] is too large to convert to an integer." << std::endl;
        }
    }
    return result;
}


void WFC::InitGrid() 
{
    surroundingTile = Tile(-1, -1, entropyKeys, seed);

    if (WFCTYPE == 0) 
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                Tile _tile = Tile(x, y, entropyKeys, seed);
                _tile.entropyList = &entropyList;
                fullGridTile[std::make_pair(x, y)] = _tile;
            }
        }
    }

    for (auto& tile : fullGridTile)
    {
        auto& _tile = tile.second;

        auto neighborIt = fullGridTile.find(std::make_pair(_tile.x, _tile.y - 1));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.up = &neighborIt->second;
        }

        neighborIt = fullGridTile.find(std::make_pair(_tile.x + 1, _tile.y));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.right = &neighborIt->second;
        }

        neighborIt = fullGridTile.find(std::make_pair(_tile.x, _tile.y + 1));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.down = &neighborIt->second;
        }

        neighborIt = fullGridTile.find(std::make_pair(_tile.x - 1, _tile.y));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.left = &neighborIt->second;
        }
    }
}

void WFC::CollapseTile()
{

    std::vector<std::pair<int, int>> _list = GetLowestEntropyList();
    if (_list.size() <= 0) return;
    std::random_device rd;
    std::mt19937 gen(*seed);
    std::uniform_int_distribution<> distribution(0, _list.size() - 1);
    int rng = distribution(gen);

    fullGridTile[_list[rng]].CollapseTile();


    tileStack.push(_list[rng]);

    auto temp = _list[rng];
    Propergate(_list[rng]);
}

void WFC::Propergate(std::pair<int,int> origin)
{

    std::queue<std::pair<int, int>> todo;
    std::set<std::pair<int, int>> done;

    auto checkAndAdd = [&](Tile* tile) {
        if (tile != nullptr && tile->entropy.size() > 1 && done.find({ tile->x, tile->y }) == done.end()) 
        {
            todo.emplace(tile->x, tile->y);
        }
    };

    // Initialize the queue with the neighbors of the origin
    checkAndAdd(fullGridTile[origin].up);
    checkAndAdd(fullGridTile[origin].right);
    checkAndAdd(fullGridTile[origin].down);
    checkAndAdd(fullGridTile[origin].left);

    while (!todo.empty()) 
    {
        const auto current = todo.front();
        Tile* tile = &fullGridTile[current];
        int startingEntropy = tile->entropy.size();
        tile->Propagate();
        done.emplace(current);
        todo.pop();

        // If entropy was reduced, check neighbors
        if (startingEntropy > tile->entropy.size()) 
        {
            checkAndAdd(tile->up);
            checkAndAdd(tile->right);
            checkAndAdd(tile->down);
            checkAndAdd(tile->left);
        }
    }
}

std::vector<std::pair<int, int>> WFC::GetLowestEntropyList() {
    // Initialize an empty vector to store the positions of tiles with the lowest entropy
    std::vector<std::pair<int, int>> lowestEntropyTiles;

    // Set initial lowest entropy to the highest possible value
    int minimumEntropy = std::numeric_limits<int>::max();

    // Iterate through all tiles in the grid
    for (const auto& tileEntry : fullGridTile) {
        const Tile& currentTile = tileEntry.second; // Get the current tile

        // Skip this iteration if the tile is already collapsed
        if (currentTile.collapsed) {
            continue;
        }

        // Get the entropy size of the current tile
        int currentEntropy = currentTile.entropy.size();

        // Check if the current tile's entropy is lower than the known minimum
        if (currentEntropy < minimumEntropy) {
            // Update the minimum entropy
            minimumEntropy = currentEntropy;

            // Since a new minimum entropy is found, clear the previous list
            lowestEntropyTiles.clear();

            // Add the current tile's position to the list
            lowestEntropyTiles.push_back(tileEntry.first);
        }
        else if (currentEntropy == minimumEntropy) {
            // If the current entropy matches the minimum, just add the position
            lowestEntropyTiles.push_back(tileEntry.first);
        }
    }

    // Return the list of positions with the lowest entropy
    return lowestEntropyTiles;
}

std::string WFC::ReverseString(const std::string& str)
{
    return std::string(str.rbegin(), str.rend());
}

void WFC::writeToJson(const std::vector<Tile>& tiles, const std::string& filename)
{
    nlohmann::json j;
    j["Grid"] = nlohmann::json::array();

    for (const auto& tile : tiles)
    {
        nlohmann::json tileObj;

        nlohmann::json entropyArray = nlohmann::json::array();
        for (int value : tile.entropy)
        {
            entropyArray.push_back(value);
        }
        tileObj["Entropy"] = entropyArray; 

        tileObj["EntropyCount"] = tile.entropy.size(); 

        tileObj["X"] = tile.x;
        tileObj["Y"] = tile.y;

        j["Grid"].push_back(tileObj);
    }

    j["EntropyList"] = nlohmann::json::object(); 
    for (auto it = entropyList.begin(); it != entropyList.end(); ++it)
    {
        nlohmann::json ruleObj;
        ruleObj["Up"] = it->second.up;
        ruleObj["Right"] = it->second.right;
        ruleObj["Down"] = ReverseString(it->second.down);
        ruleObj["Left"] = ReverseString(it->second.left);
        ruleObj["spritePos"] = it->second.spritePosition;

        j["EntropyList"][std::to_string(it->first)] = ruleObj; 
    }

    std::ofstream o(filename);
    o << std::setw(4) << j << std::endl;
}