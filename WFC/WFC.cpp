#include "WFC.h"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>

using namespace std::chrono;

WFC::WFC(int _gridWidth, int _gridHeight, int _seed, std::string inputFile)
{
    inputFile = "input/" + inputFile;
    std::string temp = "data\\Data" + std::to_string(_seed) + ".json";
    seed = new int;
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
        CollapseTile();
        count++;

        for (int x = 0; x < width; x++) 
        {
            for (int y = 0; y < height; y++) 
            {
                if (Grid[{x, y}]->collapsed) count++;
            }
        }

        //std::cout << count << std::endl;
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
                Grid[tile]->Reset();
                tileStack.pop();
            }
            for (auto tile : Grid) 
            {
                if (!Grid[(tile.first)]->collapsed)
                {
                    Grid[(tile.first)]->Reset();
                }
            }
            
            lastCount = 0;
        }

        lastCount = count;
        count = 0;

    }

    //std::cout << "starting right" << std::endl;

    std::vector<Tile*> tiles;
    for (auto tile : Grid) tiles.push_back(tile.second);
    //std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;
    writeToJson(tiles, temp);
}

WFC::WFC(int _gridWidth, int _gridHeight, int _regionWidth, int _regionHeight, int _seed, std::string inputFile,bool nested)
{
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
    
    bool regionsCompleted = false;
    int cunt = 2;
    while(!regionsCompleted)
    {
        //for (auto reg : regionGrid)
        //{
        //    if(!reg.second->completed)std::cout << reg.first.first << ", " << reg.first.second << " " << reg.second->entropy << std::endl;
        //}

        std::vector<std::pair<int, int>> _list;

        if(nested)
        _list = GetLowestRegionEntropyList();
        else
        for (auto reg : regionGrid) 
                {
                if(!reg.second->completed) _list.push_back(reg.first);
                }

        if (_list.size() <= 0)
        {
            regionsCompleted = true;
            continue;
        }
        std::random_device rd;
        std::mt19937 gen((*seed)++);
        std::uniform_int_distribution<> distribution(0, _list.size() - 1);
        int rng = distribution(gen);
        currentRegion = _list[rng];
        
        useableGrid = regionGrid[_list[rng]]->Grid;

        int count = 0;
        int lastCount = -1;
        bool completed = false;
        for (int i = 0; i < tileStack.size(); i++)
        {
            tileStack.pop();
        }
        failCount = 0;

        while (!completed)
        {
            (*seed)++;
            CollapseTile();
            count++;

            for (int x = regionGrid[currentRegion]->startX; x < regionGrid[currentRegion]->startX+ regionGrid[currentRegion]->width; x++)
            {
                for (int y = regionGrid[currentRegion]->startY; y < regionGrid[currentRegion]->startY + regionGrid[currentRegion]->height; y++)
                {
                    if (useableGrid[{x, y}]->collapsed) count++;
                }
            }
            if (count > regionGrid[currentRegion]->width * regionGrid[currentRegion]->height)
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
                        useableGrid[tile]->Reset();
                        tileStack.pop();
                    }
                    else failCount = 0;
                }
                for (auto tile : useableGrid)
                {
                    if (!useableGrid[tile.first]->collapsed)
                    {
                        useableGrid[tile.first]->Reset();
                    }
                }
                for (auto tile : useableGrid)
                {
                    if (!useableGrid[tile.first]->collapsed)
                    {
                        useableGrid[tile.first]->Propagate();
                    }
                }
            }

            lastCount = count;
            count = 0;
        }

        regionGrid[currentRegion]->completed = true;


        /*std::vector<Tile*> tiles;
        for (auto tile : Grid) tiles.push_back(tile.second);
        temp = "data\\Data" + std::to_string(cunt) + ".json";
        writeToJson(tiles, temp);
        temp = "data\\Data1.json";
        cunt++;*/
    }

    std::vector<Tile*> tiles;
    for (auto tile : Grid) tiles.push_back(tile.second);
    //std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;
    writeToJson(tiles, temp);
}

WFC::WFC(int _gridWidth, int _gridHeight, int _regionWidth, int _regionHeight, int _stichedSize, int _seed, std::string inputFile)
{
    WFCTYPE = 2;
    inputFile = "input/" + inputFile;
    std::string temp = "data\\Data" + std::to_string(_seed) + ".json";
    seed = &_seed;
    width = _gridWidth;
    height = _gridHeight;
    regionWidth = _regionWidth;
    regionHeight = _regionHeight;
    stichSize = _stichedSize;
    
    //std::cout << "Init rules" << std::endl;
    InitRules(inputFile);
    //std::cout << "Init Grid" << std::endl;
    InitGrid();

    bool regionsCompleted = false;
    int RegionCount = -1;
    int cunt = 2;
    while (!regionsCompleted)
    {
        RegionCount++;
        
        if (RegionCount >= regionsList.size()) 
        {
            regionsCompleted = true;
            continue;
        }

        currentRegion = regionsList[RegionCount];

        useableGrid = regionGrid[currentRegion]->Grid;

        int count = 0;
        int lastCount = -1;
        bool completed = false;
        for (int i = 0; i < tileStack.size(); i++)
        {
            tileStack.pop();
        }
        failCount = 0;

        while (!completed)
        {
            (*seed)++;
            CollapseTile();
            count++;

            for (int x = regionGrid[currentRegion]->startX; x < regionGrid[currentRegion]->startX + regionGrid[currentRegion]->width; x++)
            {
                for (int y = regionGrid[currentRegion]->startY; y < regionGrid[currentRegion]->startY + regionGrid[currentRegion]->height; y++)
                {
                    if (useableGrid[{x, y}]->collapsed) count++;
                }
            }
            if (count > regionGrid[currentRegion]->width * regionGrid[currentRegion]->height)
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
                        useableGrid[tile]->Reset();
                        tileStack.pop();
                    }
                    else failCount = 0;
                }
                for (auto tile : useableGrid)
                {
                    if (!useableGrid[tile.first]->collapsed)
                    {
                        useableGrid[tile.first]->Reset();
                    }
                }
                for (auto tile : useableGrid)
                {
                    if (!useableGrid[tile.first]->collapsed)
                    {
                        useableGrid[tile.first]->Propagate();
                    }
                }
            }

            lastCount = count;
            count = 0;
        }

        regionGrid[currentRegion]->completed = true;


        /*std::vector<Tile*> tiles;
        for (auto tile : Grid) tiles.push_back(tile.second);
        temp = "data\\Data" + std::to_string(cunt) + ".json";
        writeToJson(tiles, temp);
        temp = "data\\Data1.json";
        cunt++;*/
    }

    std::vector<Tile*> tiles;
    for (auto tile : Grid) tiles.push_back(tile.second);
    //std::cout << "data\\Data" + std::to_string(_seed) + ".json" << std::endl;
    writeToJson(tiles, temp);
}

WFC::~WFC()
{

    for (auto reg : regionGrid)
    {
        delete(reg.second);
    }
    for (auto reg : Grid)
    {
        delete(reg.second);
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

}

bool compareRules(const Rule& a, const Rule& b) {
    if (a.up != b.up) {
        return a.up < b.up;
    }
    else if (a.right != b.right) {
        return a.right < b.right;
    }
    else if (a.down != b.down) {
        return a.down < b.down;
    }
    else {
        return a.left < b.left;
    }
}

void WFC::printNeighbours(const std::map<int, Neighbours>& neighbourRules) {
    for (const auto& neigh : neighbourRules) {
        std::cout << "Rule ID: " << neigh.first << std::endl;
        std::cout << entropyList[neigh.first].up <<"  Up: ";
        for (int n : neigh.second.up) std::cout << n << " ";
        std::cout << std::endl << entropyList[neigh.first].right << "  Right: ";
        for (int n : neigh.second.right) std::cout << n << " ";
        std::cout << std::endl << entropyList[neigh.first].down << "  Down: ";
        for (int n : neigh.second.down) std::cout << n << " ";
        std::cout << std::endl << entropyList[neigh.first].left << "  Left: ";
        for (int n : neigh.second.left) std::cout << n << " ";
        std::cout << "\n";
    }
}

void WFC::InitRules(std::string inputFile)
{
    std::vector<Rule> _rules = readCSV(inputFile);
     
    std::sort(_rules.begin(), _rules.end(), compareRules);

    for (int id = 0; id < _rules.size(); id++)
    {
        entropyList[id] = _rules[id];
    }

    for (const auto& entropy : entropyList)
    {
        entropyKeys.push_back(entropy.first);
    }
    //neighbourRules = findNeighbours(entropyList);
    //printNeighbours(neighbourRules);
}

std::map<int, Neighbours> WFC::findNeighbours(const std::map<int, Rule>& entropyList) {
    std::map<int, Neighbours> neighbourRules;

    for (const auto& rule : entropyList) {
        Neighbours neighbours;

        for (const auto& other : entropyList) {
            if (rule.second.up == other.second.down) {
                neighbours.up.push_back(other.first);
            }
            if (rule.second.right == other.second.left) {
                neighbours.right.push_back(other.first);
            }
            if (rule.second.down == other.second.up) {
                neighbours.down.push_back(other.first);
            }
            if (rule.second.left == other.second.right) {
                neighbours.left.push_back(other.first);
            }
        }

        neighbourRules[rule.first] = neighbours;
    }

    return neighbourRules;
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
        if (row.weight != 0)
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

struct RegionTemp {
    int startX, startY;
    int width, height;
};

void WFC::InitGrid() 
{
    if (WFCTYPE == 0 || WFCTYPE == 1 || WFCTYPE == 2)
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                Tile* tile = new Tile(x, y, entropyKeys, seed);
                Grid[{x, y}] = tile;
                Grid[{x, y}]->entropyList = &entropyList;
            }
        }
        for (auto tile : Grid) {
            Tile* _tile = tile.second;

            auto neighborIt = Grid.find({ _tile->x, _tile->y - 1 });
            if (neighborIt != Grid.end()) {
                _tile->up = neighborIt->second;
            }

            neighborIt = Grid.find({ _tile->x + 1, _tile->y });
            if (neighborIt != Grid.end()) {
                _tile->right = neighborIt->second;
            }

            neighborIt = Grid.find({ _tile->x, _tile->y + 1 });
            if (neighborIt != Grid.end()) {
                _tile->down = neighborIt->second;
            }

            neighborIt = Grid.find({ _tile->x - 1, _tile->y });
            if (neighborIt != Grid.end()) {
                _tile->left = neighborIt->second;
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

                std::unordered_map<std::pair<int, int>, Tile*, pair_hash> grid;

                for (int i = x; i < x + currentSubWidth; i++) {
                    for (int k = y; k < y + currentSubHeight; k++) {
                        grid[{i, k}] = Grid[{i, k}];
                    }
                }

                Region* reg = new Region(grid, currentSubWidth, currentSubHeight, x, y);
                // Directly construct the Region in the map
                for (int i = x; i < x + currentSubWidth; i++) {
                    for (int k = y; k < y + currentSubHeight; k++) {
                        // Simply copy the shared_ptr
                        Grid[{i, k}]->region = reg;
                    }
                }
                regionGrid.emplace(std::make_pair(xCount, yCount), reg);

                yCount++;
            }
            xCount++;
            yCount = 0;
        }

        for (auto& reg : regionGrid)
        {
            reg.second->GetEntropy();
        }
    }

    if (WFCTYPE == 2) 
    {
        std::vector<RegionTemp> regions;
        int currentY = 0;
        
        int gH = height; int gW = width;
        int rH = regionHeight; int rW = regionHeight;
        int sS = stichSize;
        
        for (int y = 0; y < gH; y += rH + sS) {
            for (int x = 0; x < gW; x += rW + sS) {
                RegionTemp region;
                region.startX = x;
                region.startY = y;

                // Adjust width and height if region exceeds grid boundaries
                region.width = (x + rW > gW) ? gW - x : rW;
                region.height = (y + rH > gH) ? gH - y : rH;

                regions.push_back(region);
            }
        }

        for (int y = rH; y < gH; y += rH + sS)
        {
            for (int x = rW; x < gW; x += rW + sS)
            {
                RegionTemp region;
                region.startX = x;
                region.startY = y;
                region.width = sS;
                region.height = sS;
                regions.push_back(region);
            }
        }

        for (int y = 0; y < gH; y += rH + sS) 
        {
            for (int x = 0; x < gW; x += rW + sS) 
            {
                if (x + rW < gW) 
                {
                    RegionTemp region;
                    region.startX = x + rW;
                    region.startY = y;
                    region.width = sS;
                    region.height = (y + rH > gH) ? gH - y : rH;
                    regions.push_back(region);
                }
            }
        }

        for (int y = 0; y < gH; y += rH + sS) 
        {
            for (int x = 0; x < gW; x += rW + sS) 
            {
                if (y + rH < gH) 
                {
                    RegionTemp region;
                    region.startX = x;
                    region.startY = y + rH;
                    region.width = (x + rW > gW) ? gW - x : rW;
                    region.height = sS;
                    regions.push_back(region);
                }
            }
        }


        for (auto reg : regions)
        {
            //std::cout << reg.startX << ", " << reg.startY << std::endl;
        
            int currentSubWidth = reg.width;
            int currentSubHeight = reg.height;
            int x = reg.startX;
            int y = reg.startY;
            std::unordered_map<std::pair<int, int>, Tile*, pair_hash> grid;

            for (int i = x; i < x + currentSubWidth; i++) {
                for (int k = y; k < y + currentSubHeight; k++) {
                    grid[{i, k}] = Grid[{i, k}];
                }
            }

            Region* reg = new Region(grid, currentSubWidth, currentSubHeight, x, y);
            // Directly construct the Region in the map
            for (int i = x; i < x + currentSubWidth; i++) {
                for (int k = y; k < y + currentSubHeight; k++) {
                    // Simply copy the shared_ptr
                    Grid[{i, k}]->region = reg;
                }
            }
            regionsList.push_back({ x,y });
            regionGrid.emplace(std::make_pair(x, y), reg);
        }

    }
}

void WFC::CollapseTile()
{
    std::vector<std::pair<int, int>> _list = GetLowestEntropyList();

    /*for (auto lit : _list) 
    {
        std::cout << lit.first << ", " << lit.second << std::endl;
    }*/

    if (_list.size() <= 0) return;
    std::random_device rd;
    std::mt19937 gen((*seed)++);
    std::uniform_int_distribution<> distribution(0, _list.size() - 1);
    int rng = distribution(gen);

    useableGrid[(_list[rng])]->CollapseTile();

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
    checkAndAdd(Grid[origin]->up);
    checkAndAdd(Grid[origin]->right);
    checkAndAdd(Grid[origin]->down);
    checkAndAdd(Grid[origin]->left);

    while (!todo.empty()) 
    {
        const auto current = todo.front();
        Tile* tile = Grid[current];
        int startingEntropy = tile->entropy.size();
        tile->Propagate();
        done.emplace(current);
        todo.pop();
        if (tile->region != nullptr)
        {
            tile->region->entropy -= startingEntropy - tile->entropy.size();
        }
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
        Tile* currentTile = tileEntry.second; // Get the current tile

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
            lowestEntropyTiles.push_back(tileEntry.first);
        }
        else if (currentEntropy == minimumEntropy) {
            // If the current entropy matches the minimum, just add the position
            lowestEntropyTiles.push_back(tileEntry.first);
        }
    }
    std::random_device rd;  // Non-deterministic random device
    std::mt19937 g((*seed)++);

    std::shuffle(lowestEntropyTiles.begin(), lowestEntropyTiles.end(), g);

    // Return the list of positions with the lowest entropy
    return lowestEntropyTiles;
}

std::vector<std::pair<int, int>> WFC::GetLowestRegionEntropyList() {
    // Initialize an empty vector to store the positions of tiles with the lowest entropy
    std::vector<std::pair<int, int>> lowestEntropyTiles;

    // Set initial lowest entropy to the highest possible value
    int minimumEntropy = std::numeric_limits<int>::max();

    // Iterate through all tiles in the grid
    for (const auto& tileEntry : regionGrid) {
        Region* currentTile = tileEntry.second; // Get the current tile

        // Skip this iteration if the tile is already collapsed
        if (currentTile->completed) {
            continue;
        }

        // Get the entropy size of the current tile
        int currentEntropy = currentTile->entropy;

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