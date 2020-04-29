#include "FPTree.h"

#include <algorithm>


void FPTreePrint(std::ostream& os, const FPNode* root, unsigned int deep) {
    for (unsigned int i = 0; i < deep; i++) {
        os << "  ";
    }
    if (root->item != UINT32_MAX) {
        os << root->item << " (" << root->times << ')' << std::endl;
    } else {
        os << '-' << std::endl;
    }
    for (auto child : root->children) {
        FPTreePrint(os, child, deep + 1);
    }
}

std::ostream& operator<<(std::ostream& os, const FPNode* root) {
    FPTreePrint(os, root, 0);
    return os;
}

void FPTree::putItem(const unsigned int item, const unsigned int frequency, const unsigned int orderIndex) {
    this->headerTable.emplace(item, std::make_tuple(frequency, orderIndex, std::list<FPNode*>()));
}

void FPTree::putItems(std::vector<std::pair<unsigned, unsigned>>& itemWithFrequency) {
    unsigned int i = 0;
    this->headerTable.clear();
    std::sort(itemWithFrequency.begin(), itemWithFrequency.end(),
              [](const std::pair<unsigned int, unsigned int>& a, const std::pair<unsigned int, unsigned int>& b) {
                  return b.second < a.second;});
    for (const auto item : itemWithFrequency) {
        putItem(item.first, item.second, i++);
    }
}

void FPTree::putToTree(const std::vector<unsigned>& orderedData, const unsigned int inc) {
    if (this->root == nullptr) {
        this->root = new FPNode();
    }
    FPNode* currentNode = this->root;
    for (auto data : orderedData) {
        FPNode* childNode = nullptr;
        for (auto child : currentNode->children) {
            if (child->item == data) {
                childNode = child;
                break;
            }
        }
        if (childNode == nullptr) {  // build
            childNode = new FPNode(data, currentNode);
            currentNode->children.push_back(childNode);
            std::get<2>(this->headerTable[data]).push_back(childNode);
        }
        currentNode = childNode;
        childNode->times += inc;
    }
}

void FPTree::putToTree(const std::unordered_set<unsigned>& unorderedData, const unsigned int inc) {
    std::vector<unsigned int> orderedTrans;
    orderedTrans.reserve(unorderedData.size());
    for (auto item : unorderedData) {
        if (this->headerTable.find(item) == this->headerTable.end()) {
            continue;
        }
        orderedTrans.push_back(item);
    }
    std::sort(orderedTrans.begin(), orderedTrans.end(),
              [this](unsigned int a, unsigned int b) {
                  return std::get<1>(this->headerTable[a]) < std::get<1>(this->headerTable[b]);});
    putToTree(orderedTrans, inc);
}

void FPTree::putConditional(const Pattern& conditional, const unsigned int minSupport) {
    std::unordered_map<unsigned int, unsigned int> fDict;
    for (const auto& trans : conditional) {
        for (auto item : trans.first) {
            if (fDict.find(item) == fDict.end()) {
                fDict.emplace(item, 0);
            }
            fDict[item] += trans.second;
        }
    }
    std::vector<std::pair<unsigned int, unsigned int>> fList;
    for (auto item : fDict) {
        if (item.second >= minSupport) {
            fList.emplace_back(item.first, item.second);
        }
    }
    putItems(fList);
    for (const auto& trans : conditional) {
        putToTree(trans.first, trans.second);
    }
}

FPTree::~FPTree() {
    for (const auto& item : this->headerTable) {
        for (FPNode* node : std::get<2>(item.second)) {
            delete node;
        }
    }
    delete this->root;
    this->root = nullptr;
    this->headerTable.clear();
}
