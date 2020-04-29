#pragma once
#include "../FPAlgorithm.h"
#include "FPTree.h"

class FPGrowth final : public FPAlgorithm {
private:
    FPTree fpTree;
    Pattern frequentItemsets;
    void initialTreeFList(const unsigned int minSupport);
    void initialTree();
    void patternsGenerator(const unsigned int minSupport);
    void recursivePatternsGenerator(std::pair<std::unordered_set<unsigned int>, unsigned int>& trans, 
                                    FPTree& tree, const unsigned int minSupport);
public:
    FPGrowth(DataLoader& dataLoader) : FPAlgorithm(dataLoader) { };
    void findFrequentItemsets(Pattern& frequentItemsets, const unsigned int minSupport,
                              bool verbose = false) override;
};

