#pragma once
#include <cstdint>
#include <ostream>
#include <unordered_map>
#include <vector>
#include <list>

#include "../Apriori/Apriori.h"

struct FPNode {
    unsigned int item;
    unsigned int times = 0;
    FPNode* parent = nullptr;
    std::vector<FPNode*> children;

    FPNode() : item(UINT32_MAX) {
    };
    FPNode(const unsigned int i) : item(i) {
    };
    FPNode(const unsigned int i, FPNode* p) : item(i), parent(p) {
    };
};

extern std::ostream& operator<<(std::ostream& os, const FPNode* root);

class FPTree {
public:
    std::unordered_map<unsigned int, std::tuple<unsigned int, unsigned int, std::list<FPNode*>>> headerTable;
    FPNode* root = nullptr;
    FPTree() = default;
    FPTree(const Pattern& conditional, const unsigned int minSupport) {
        putConditional(conditional, minSupport);
    };
    void putItem(const unsigned int item, const unsigned int frequency, const unsigned int orderIndex);
    void putItems(std::vector<std::pair<unsigned int, unsigned int>>& itemWithFrequency);
    void putToTree(const std::vector<unsigned int>& orderedData, const unsigned int inc = 1);
    void putToTree(const std::unordered_set<unsigned int>& unorderedData, const unsigned int inc = 1);
    void putConditional(const Pattern& conditional, const unsigned int minSupport);
    ~FPTree();
};

