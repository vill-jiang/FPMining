#include "Apriori.h"

#include <chrono>
#include <iostream>
#include <unordered_map>


void Apriori::loadInitCandidates() {
    this->candidateVec.clear();
    std::unordered_map<unsigned int, unsigned int> counter;
    this->dataLoader.reset();
    while(this->dataLoader.hasNext()) {
        auto tmpSet = this->dataLoader.getNext();
        for (auto item : tmpSet) {
            if (counter.find(item) == counter.end()) {
                counter.emplace(item, 0);
            }
            counter[item]++;
        }
    }
    this->candidateVec.emplace_back(Pattern());
    for (auto item: counter) {
        std::unordered_set<unsigned int> tmpSet;
        tmpSet.emplace(item.first);
        this->candidateVec[0].emplace(tmpSet, item.second);
    }
}

bool Apriori::loadNextCandidates(bool verbose) {
    // join and get n+1 C
    auto startWith = std::chrono::system_clock::now();
    const int sizeBefore = this->candidateVec.size();
    this->candidateVec.emplace_back(Pattern());
    for (auto i = this->candidateVec[sizeBefore - 1].begin();
         i != this->candidateVec[sizeBefore - 1].end(); ++i) {
        for (auto j = i; j != this->candidateVec[sizeBefore - 1].end(); ++j) {
            std::unordered_set<unsigned int> tmpSet(i->first);
            tmpSet.insert(j->first.begin(), j->first.end());
            if (tmpSet.size() == i->first.size() + 1) {
                // remove subset don't exist in K
                bool existFlag = true;
                for (auto item : tmpSet) {
                    auto tmp(tmpSet);
                    tmp.erase(item);
                    if (this->candidateVec[sizeBefore - 1].find(tmp) == this->candidateVec[sizeBefore - 1].end()) {
                        existFlag = false;
                        break;
                    }
                }
                if (existFlag) {
                    this->candidateVec.back()[tmpSet] = 0;
                }
            }
        }
    }
    if (this->candidateVec.back().empty()) {
        this->candidateVec.pop_back();
        return false;
    }
    const std::chrono::duration<double> generateDiff = std::chrono::system_clock::now() - startWith;
    // scan
    startWith = std::chrono::system_clock::now();
    this->dataLoader.reset();
    while (this->dataLoader.hasNext()) {
        auto dataSetItem = this->dataLoader.getNext();
        if (dataSetItem.size() < this->candidateVec.size()) {
            continue;
        }
        for (auto it = this->candidateVec.back().begin(); it != this->candidateVec.back().end(); ++it) {
            bool existFlag = true;
            for (auto i : it->first) {
                if (dataSetItem.find(i) == dataSetItem.end()) {
                    existFlag = false;
                    break;
                }
            }
            if (existFlag) {
                it->second++;
            }
        }
    }
    const std::chrono::duration<double> scanDiff = std::chrono::system_clock::now() - startWith;
    if (verbose) {
        std::cout << "        Generate: " << generateDiff.count() << " s" << std::endl;
        std::cout << "        Scan: " << scanDiff.count() << " s" << std::endl;
    }
    return true;
}

bool Apriori::removeLessSupport(const unsigned int minSupport) {
    for (auto it = this->candidateVec.back().begin();
         it != this->candidateVec.back().end(); ) {
        if (it->second < minSupport) {
            it = this->candidateVec.back().erase(it);
        } else {
            ++it;
        }
    }
    if (this->candidateVec.back().empty()) {
        this->candidateVec.pop_back();
        return false;
    }
    return true;
}

void Apriori::findFrequentItemsets(Pattern& frequentItemsets, const unsigned int minSupport,
                                   bool verbose) {
    if (verbose) {
        std::cout << "Apriori consuming:" << std::endl << "    Round initial: " << std::endl;
    }
    auto startWith = std::chrono::system_clock::now();
    loadInitCandidates();
    removeLessSupport(minSupport);
    //std::chrono::duration<double> diff = std::chrono::system_clock::now() - startWith;
    int round = 1;
    do {
        std::chrono::duration<double> diff = std::chrono::system_clock::now() - startWith;
        if (verbose) {
            std::cout << "      Total: " << diff.count() << "s, get " << this->candidateVec.back().size() << " itemsets" << std::endl
                      << "    Round " << ++round << ": " << std::endl;
        }
        startWith = std::chrono::system_clock::now();
    } while (loadNextCandidates(verbose) && removeLessSupport(minSupport));
    if (verbose) {
        std::cout << "      Empty! " << std::endl;
    }
    for (auto candidate : this->candidateVec) {
        frequentItemsets.insert(candidate.begin(), candidate.end());
    }
}
