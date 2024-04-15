#include "WFC.h"

WFC::WFC(int _gridHeight, int _gridWidth)
{
    width = _gridWidth;
    height = _gridHeight;
    std::cout << "Init rules" << std::endl;
    InitRules();
    std::cout << "Init Grid" << std::endl;
    InitGrid();
    int count = 0;
    bool completed = false;
    while (!completed)
    {
        CollapseTile();
        Propergate();
        count++;
        if (count >= 5) return;
    }
}

WFC::~WFC()
{

}

void WFC::InitRules()
{
	std::vector<std::string> _types = { "AAA","ABA","ACA"};
    std::vector<Rule> _rules;
    //int id = 0;
    /*for (const auto& rule1 : _ruleTypes) 
    {
        for (const auto& rule2 : _ruleTypes) 
        {
            for (const auto& rule3 : _ruleTypes) 
            {
                for (const auto& rule4 : _ruleTypes) 
                {
                    Rule rule = { rule1, rule2, rule3, rule4};
                    entropyList[id++] = rule;
                }
            }
        }
    }

    for (const auto& entropy : entropyList) {
        entropyKeys.push_back(entropy.first);
    }*/

    Rule rule = { _types[0],_types[0],_types[1] ,_types[1] };
    _rules.push_back(rule);
    rule = { _types[0],_types[1],_types[1] ,_types[0] };
    _rules.push_back(rule);
    rule = { _types[1],_types[1],_types[0] ,_types[0] };
    _rules.push_back(rule);
    rule = { _types[1],_types[0],_types[0] ,_types[1] };
    _rules.push_back(rule);
    rule = { _types[0],_types[1],_types[1] ,_types[1] };
    _rules.push_back(rule);
    rule = { _types[1],_types[0],_types[1] ,_types[1] };
    _rules.push_back(rule);
    rule = { _types[1],_types[1],_types[0] ,_types[1] };
    _rules.push_back(rule);
    rule = { _types[1],_types[1],_types[1] ,_types[0] };
    _rules.push_back(rule);

    rule = { _types[0],_types[0],_types[2] ,_types[2] };
    _rules.push_back(rule);
    rule = { _types[0],_types[2],_types[2] ,_types[0] };
    _rules.push_back(rule);
    rule = { _types[2],_types[2],_types[0] ,_types[0] };
    _rules.push_back(rule);
    rule = { _types[2],_types[0],_types[0] ,_types[2] };
    _rules.push_back(rule);
    rule = { _types[0],_types[2],_types[2] ,_types[2] };
    _rules.push_back(rule);
    rule = { _types[2],_types[0],_types[2] ,_types[2] };
    _rules.push_back(rule);
    rule = { _types[2],_types[2],_types[0] ,_types[2] };
    _rules.push_back(rule);
    rule = { _types[2],_types[2],_types[2] ,_types[0] };
    _rules.push_back(rule);

    for (int id = 0; id < _rules.size(); id++)
    {
        entropyList[id] = _rules[id];
    }

    for (const auto& entropy : entropyList) 
    {
        entropyKeys.push_back(entropy.first);
    }

    //// Print the contents of the BaseRules map
    //for (const auto& pair : entropyList)
    //{
    //    std::cout << "ID: " << pair.first << ", Rule: "
    //        << pair.second.up << " "
    //        << pair.second.right << " "
    //        << pair.second.down << " "
    //        << pair.second.left << std::endl;
    //}
}

void WFC::InitGrid() 
{
    surroundingTile = Tile(-1, -1, entropyKeys);

    if (WFCTYPE == 0) 
    {
        for (int x = 0; x < width; x++)
        {
            for (int y = 0; y < height; y++)
            {
                Tile _tile = Tile(x, y, entropyKeys);
                _tile.entropyList = &entropyList;
                fullGridTile[std::make_pair(x, y)] = _tile;
            }
        }
    }

    //std::cout << fullGridTile.size() << std::endl;
    //// Print out the created tiles
    //for (const auto& tile : fullGridTile)
    //{
    //    auto _tile = tile.second;
    //    std::cout << "Tile at (" << _tile.x << ", " << _tile.y << "), Entropy Keys: ";
    //    for (int key : _tile.entropy)
    //    {
    //        std::cout << key << " ";
    //    }
    //    std::cout << std::endl;
    //}

    for (auto& tile : fullGridTile)
    {
        auto& _tile = tile.second;

        auto neighborIt = fullGridTile.find(std::make_pair(_tile.x, _tile.y - 1));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.up = &neighborIt->second;
        }
        else 
        {
            _tile.up = new Tile(-1, -1, entropyKeys); 
        }

        neighborIt = fullGridTile.find(std::make_pair(_tile.x + 1, _tile.y));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.right = &neighborIt->second;
        }
        else 
        {
            _tile.right = new Tile(-1, -1, entropyKeys); 
        }

        neighborIt = fullGridTile.find(std::make_pair(_tile.x, _tile.y + 1));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.down = &neighborIt->second;
        }
        else 
        {
            _tile.down = new Tile(-1, -1, entropyKeys);
        }

        neighborIt = fullGridTile.find(std::make_pair(_tile.x - 1, _tile.y));
        if (neighborIt != fullGridTile.end()) 
        {
            _tile.left = &neighborIt->second;
        }
        else 
        {
            _tile.left = new Tile(-1, -1, entropyKeys);
        }
    }

    /*for (const auto& tile : fullGridTile) 
    {
        const auto& _tile = tile.second;
        std::cout << "Tile at (" << _tile.x << ", " << _tile.y << "): " << std::endl;
        std::cout << "Up: (" << _tile.up->x << ", " << _tile.up->y << ")" << std::endl;
        std::cout << "Right: (" << _tile.right->x << ", " << _tile.right->y << ")" << std::endl;
        std::cout << "Down: (" << _tile.down->x << ", " << _tile.down->y << ")" << std::endl;
        std::cout << "Left: (" << _tile.left->x << ", " << _tile.left->y << ")" << std::endl;
        std::cout << std::endl;
    }*/
}

void WFC::CollapseTile() 
{
    //std::cout << GetLowEntropyList().size() << std::endl;
    std::vector<std::pair<int, int>> _list = GetLowestEntropyList();
    if (_list.size() <= 0) return;
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_int_distribution<> distribution(0, _list.size()-1);
    int rng = distribution(gen);
    //std::cout << _list[rng].first << ", " << _list[rng].second << std::endl;
    fullGridTile[_list[rng]].CollapseTile();
   /* std::cout << fullGridTile[_list[rng]].entropy.size() << std::endl;
    std::cout << fullGridTile[_list[rng]].entropy[0] << std::endl;*/
}

void WFC::Propergate()
{
    auto _list = GetLowEntropyList();
    fullGridTile[_list[0]].Propogate();
}

std::vector<std::pair<int, int>> WFC::GetLowestEntropyList() 
{
    std::vector<std::pair<int, int>> lowEntropyTiles;
    int lowestEntropy = std::numeric_limits<int>::max();

    for (const auto& tileEntry : fullGridTile) {
        const Tile& tile = tileEntry.second;

        // Check if entropy size is greater than 1 and lower than lowestEntropy
        if (tile.entropy.size() > 1 && tile.entropy.size() < lowestEntropy) {
            lowestEntropy = tile.entropy.size();

            lowEntropyTiles.clear();
            lowEntropyTiles.push_back(tileEntry.first);
        }
        else if (tile.entropy.size() == lowestEntropy) {
            lowEntropyTiles.push_back(tileEntry.first);
        }
    }

    return lowEntropyTiles;
}

std::vector<std::pair<int, int>> WFC::GetLowEntropyList()
{
    // Intermediate vector to hold tiles and their entropy sizes for sorting
    std::vector<std::pair<std::pair<int, int>, int>> intermediateSortableTiles;

    // Fill the intermediate vector with your data
    for (const auto& tileEntry : fullGridTile) {
        int entropySize = tileEntry.second.entropy.size();
        if (entropySize > 1) {
            intermediateSortableTiles.emplace_back(tileEntry.first, entropySize);
        }
    }

    // Sort the intermediate vector based on entropy size
    std::sort(intermediateSortableTiles.begin(), intermediateSortableTiles.end(),
        [](const auto& a, const auto& b) {
            return a.second < b.second; // Sort based on entropy size
        });

    
    // Final vector to hold just the coordinates of the tiles, post-sort
    std::vector<std::pair<int, int>> sortedTiles;

    // Extract just the tile coordinates into the final vector
    for (const auto& item : intermediateSortableTiles) {
        sortedTiles.push_back(item.first); // `item.first` is the std::pair<int, int> part
    }
    /*for (const auto& tile : sortedTiles)
    {
        std::cout << "Tile at (" << tile.first << ", " << tile.second
            << ") has entropy size of " << fullGridTile[tile].entropy.size() << " "
            << fullGridTile[tile].entropy[0] << std::endl;
    }*/
    // Now sortedTiles contains only the coordinates of the tiles, sorted by their entropy sizes
    return sortedTiles;
}