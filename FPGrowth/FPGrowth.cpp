#include "FPGrowth.h"
#include <algorithm>
#include <chrono>
#include <iostream>

void FPGrowth::initialTreeFList(const unsigned int minSupport) {
    std::unordered_map<unsigned int, unsigned int> fDict;
    this->dataLoader.reset();
    while(this->dataLoader.hasNext()) {
        auto& trans = this->dataLoader.getNext();
        for (auto item: trans) {
            if (fDict.find(item) == fDict.end()) {
                fDict.emplace(item, 0);
            }
            //try {
            //    fDict.at(item);
            //} catch (...) {
            //    fDict.emplace(item, 0);
            //}
            fDict[item]++;
        }
    }
    std::vector<std::pair<unsigned int, unsigned int>> fList;
    for (auto item : fDict) {
        if (item.second >= minSupport) {
            fList.emplace_back(item.first, item.second);
        }
    }
    this->fpTree.putItems(fList);
}

void FPGrowth::initialTree() {
    this->dataLoader.reset();
    while(this->dataLoader.hasNext()) {
        auto dataTrans = this->dataLoader.getNext();
        this->fpTree.putToTree(dataTrans);
    }
}

void FPGrowth::recursivePatternsGenerator(std::pair<std::unordered_set<unsigned int>, unsigned int>& trans,
                                          FPTree& tree, const unsigned int minSupport) {
    if (!trans.first.empty() && trans.second >= minSupport) {
        this->frequentItemsets.emplace(trans.first, trans.second);
    }
    for (auto& row : tree.headerTable) {
        auto item = row.first;
        //auto& newTrans = trans;
        const auto oldFrequent = trans.second;
        trans.first.emplace(item);
        trans.second = std::get<0>(row.second);
        // generate conditional pattern
        Pattern conditional;
        for (auto node : std::get<2>(row.second)) {
            std::unordered_set<unsigned int> tmpSet;
            FPNode* currentNode = node->parent;
            while (currentNode != tree.root) {
                tmpSet.emplace(currentNode->item);
                currentNode = currentNode->parent;
            }
            if (tmpSet.empty() && trans.second >= minSupport) {
                this->frequentItemsets.emplace(trans.first, trans.second);
                continue;
            }
            conditional.emplace(tmpSet, node->times);
        }
        // FP
        if (!conditional.empty()) {
            FPTree conditionalTree(conditional, minSupport);
            recursivePatternsGenerator(trans, conditionalTree, minSupport);
        }
        trans.first.erase(item);
        trans.second = oldFrequent;
    }
}

void FPGrowth::patternsGenerator(const unsigned int minSupport) {
    std::pair<std::unordered_set<unsigned int>, unsigned int> t;
    recursivePatternsGenerator(t, this->fpTree, minSupport);
}

void FPGrowth::findFrequentItemsets(Pattern& frequentItemsets, const unsigned int minSupport,
                                    bool verbose) {
    auto startWith = std::chrono::system_clock::now();
    initialTreeFList(minSupport);
    initialTree();
    const std::chrono::duration<double> initialTreeDiff = std::chrono::system_clock::now() - startWith;
    startWith = std::chrono::system_clock::now();
    patternsGenerator(minSupport);
    frequentItemsets = this->frequentItemsets;
    const std::chrono::duration<double> patternsGenerateDiff = std::chrono::system_clock::now() - startWith;
    if (verbose) {
        std::cout << "FPGrowth consuming: " << std::endl
            << "    Initial fp-tree consuming: " << initialTreeDiff.count() << " s" << std::endl
            << "    Generate patterns consuming: " << patternsGenerateDiff.count() << " s" << std::endl;
    }
}
