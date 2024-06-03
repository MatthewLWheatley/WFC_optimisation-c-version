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
    useableGrid = Grid;
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
                if (Grid.get(x,y)->collapsed) count++;
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
                Grid.get(tile)->Reset();
                tileStack.pop();
            }
            for (auto tile : Grid) 
            {
                if (!tile->collapsed)
                {
                    tile->Reset();
                }
            }
            
            lastCount = 0;
        }

        lastCount = count;
        count = 0;

    }

    //std::cout << "starting right" << std::endl;

    std::vector<Tile*> tiles;
    for (auto tile : Grid) tiles.push_back(tile);
    //std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;
    writeToJson(tiles, temp);
}

WFC::WFC(int _gridHeight, int _gridWidth, int _regionHeight, int _regionWidth, int _seed, std::string inputFile)
{
    int shit = _seed;
    WFCTYPE = 1;
    inputFile = "input/" + inputFile;
    std::string temp = "data\\Data" + std::to_string(_seed) + ".json";
    seed = &_seed;
    width = _gridWidth;
    height = _gridHeight;
    regionWidth = _regionWidth;
    regionHeight = _regionHeight;
    //std::cout << "Init rules" << std::endl;
    InitRules(inputFile);
    //std::cout << "Init Grid" << std::endl;
    InitGrid();
    for (auto& reg : regionGrid)
    {
        useableGrid = reg.second.Grid;
        //reg.second.outputdata();

        int count = 0;
        int lastCount = -1;
        bool completed = false;
        for (int i = 0; i < tileStack.size(); i++)
        {
            tileStack.pop();
        }
        failCount = 0;
        int cunt = 0;
        while (!completed)
        {
            (*seed)++;
            CollapseTile();
            //std::vector<Tile*> tiles;
            //for (auto tile : Grid) tiles.push_back(tile);
            ////std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;

            //temp = "data\\Data" + std::to_string(_seed) + ".json";
            //writeToJson(tiles, temp);


            count++;

            for (int x = reg.second.startX; x < reg.second.startX+reg.second.width; x++)
            {
                for (int y = reg.second.startY; y < reg.second.startY + reg.second.height; y++)
                {
                    if (useableGrid.get(x, y)->collapsed) count++;
                }
            }

            if (count > reg.second.width * reg.second.height)
            {
                completed = true;
            }
            if (count == lastCount)
            {
                if (lastHighest < count) failCount = 0;
                lastHighest = count;
                failCount++;
                std::cout << "failcount: " << failCount << std::endl;
                //std::cout << reg.second.startX << " " << reg.second.width << " " << reg.second.startY << " " << reg.second.height << " " << count << std::endl;
                for (int i = 0; i < failCount; i++)
                {
                    if (tileStack.size() > 0)
                    {
                        auto tile = tileStack.top();
                        useableGrid.get(tile)->Reset();
                        tileStack.pop();
                    }
                    else failCount = 0;
                }
                for (auto tile : useableGrid)
                {
                    if (!tile->collapsed)
                    {
                        tile->Reset();
                    }
                }
                for (auto tile : useableGrid)
                {
                    if (!tile->collapsed)
                    {
                        tile->Propagate();
                    }
                }
            }

            lastCount = count;
            count = 0;
        }
    }
    //std::cout << "starting right" << std::endl;


    std::vector<Tile*> tiles;
    for (auto tile : Grid) tiles.push_back(tile);
    //std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;
    writeToJson(tiles, temp);
}

WFC::~WFC()
{


    for (auto tile : Grid)
    {
        delete(tile);
    }

    Grid.clear();
    useableGrid.clear();
    regionGrid.clear();
    entropyList.clear();
    entropyKeys.clear();
    while (!tileStack.empty())
    {
        tileStack.pop();
    }

    {
        std::unordered_map<std::pair<int, int>, Region, pair_hash>().swap(regionGrid);
        std::map<int, Rule>().swap(entropyList);
        std::vector<int>().swap(entropyKeys);
    }

}

void WFC::InitRules(std::string inputFile)
{
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
        if(row.weight != 0)
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
    if (WFCTYPE == 0 || WFCTYPE == 1)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                Tile* tile = new Tile(x, y, entropyKeys, seed);
                Grid.insert(x, y, tile);//= tile;
                Grid.get({x, y})->entropyList = &entropyList;
            }
        }

        for (auto tile : Grid) {
            Tile* _tile = tile;

            Tile* neighbor = Grid.get(_tile->x, _tile->y - 1);
            if (neighbor != nullptr) {
                _tile->up = neighbor;
            }

            neighbor = Grid.get(_tile->x + 1, _tile->y);
            if (neighbor != nullptr) {
                _tile->right = neighbor;
            }

            neighbor = Grid.get(_tile->x, _tile->y + 1);
            if (neighbor != nullptr) {
                _tile->down = neighbor;
            }

            neighbor = Grid.get(_tile->x - 1, _tile->y);
            if (neighbor != nullptr) {
                _tile->left = neighbor;
            }
        }
    }

    if (WFCTYPE == 1)
    {
        int xCount = 0;
        int yCount = 0;
        for (int y = 0; y < height; y += regionHeight)
        {
            for (int x = 0; x < width; x += regionWidth)
            {
                int currentSubWidth = (x + regionWidth > width) ? (width - x) : regionWidth;
                int currentSubHeight = (y + regionHeight > height) ? (height - y) : regionHeight;

                TileGrid grid;

                for (int i = x; i < x + currentSubWidth; i++) {
                    for (int k = y; k < y + currentSubHeight; k++) {
                        // Simply copy the shared_ptr
                        grid.insert(i,k,Grid.get(i, k));
                    }
                }

                // Directly construct the Region in the map
                regionGrid.emplace(std::make_pair(xCount, yCount), Region(grid, currentSubWidth, currentSubHeight,x,y));

                yCount++;
            }
            xCount++;
        }
        /*for (auto& reg : regionGrid)
        {
            reg.second.outputdata();
        }*/
    }
}

void WFC::CollapseTile()
{
    std::vector<std::pair<int, int>> _list = GetLowestEntropyList();

    /*for (auto lit : _list)
    {
        std::cout << "Done: " << lit.first << ", " << lit.second << std::endl;
    }*/

    if (_list.size() <= 0) return;
    //std::cout << "shit " << std::endl;
    std::random_device rd;
    std::mt19937 gen(*seed);
    std::uniform_int_distribution<> distribution(0, _list.size() - 1);
    int rng = distribution(gen);

    useableGrid.get(_list[rng])->CollapseTile();

    tileStack.push(_list[rng]);

    auto temp = _list[rng];
    //std::cout << temp.first << " " << temp.second << std::endl;
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
    checkAndAdd(Grid.get(origin)->up);
    checkAndAdd(Grid.get(origin)->right);
    checkAndAdd(Grid.get(origin)->down);
    checkAndAdd(Grid.get(origin)->left);

    while (!todo.empty()) 
    {
        const auto current = todo.front();
        Tile* tile = Grid.get(current);
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
    for (const auto& tileEntry : useableGrid) {
        Tile* currentTile = tileEntry; // Get the current tile

        // Skip this iteration if the tile is already collapsed
        if (currentTile->collapsed) {
            continue;
        }

        // Get the entropy size of the current tile
        int currentEntropy = currentTile->entropy.size();

        // Check if the current tile's entropy is lower than the known minimum
        if (currentEntropy < minimumEntropy) {
            // Update the minimum entropy
            minimumEntropy = currentEntropy;

            // Since a new minimum entropy is found, clear the previous list
            lowestEntropyTiles.clear();

            // Add the current tile's position to the list
            lowestEntropyTiles.push_back({ tileEntry->x,tileEntry->y });
        }
        else if (currentEntropy == minimumEntropy) {
            // If the current entropy matches the minimum, just add the position
            lowestEntropyTiles.push_back({ tileEntry->x,tileEntry->y });
        }
    }

    // Return the list of positions with the lowest entropy
    std::random_device rd;
    std::mt19937 gen(*seed);
    std::shuffle(lowestEntropyTiles.begin(), lowestEntropyTiles.end(), gen);
    std::shuffle(lowestEntropyTiles.begin(), lowestEntropyTiles.end(), gen);
    std::shuffle(lowestEntropyTiles.begin(), lowestEntropyTiles.end(), gen);
    return lowestEntropyTiles;
}

std::string WFC::ReverseString(const std::string& str)
{
    return std::string(str.rbegin(), str.rend());
}

void WFC::writeToJson(const std::vector<Tile*> tiles, const std::string& filename)
{
    nlohmann::json j;
    j["Grid"] = nlohmann::json::array();

    for (const auto& tile : tiles)
    {
        nlohmann::json tileObj;

        nlohmann::json entropyArray = nlohmann::json::array();
        for (int value : tile->entropy)
        {
            entropyArray.push_back(value);
        }
        tileObj["Entropy"] = entropyArray; 

        tileObj["EntropyCount"] = tile->entropy.size();

        tileObj["X"] = tile->x;
        tileObj["Y"] = tile->y;

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