#pragma once
#include "../FPAlgorithm.h"
#include "../DataLoader/DataLoader.h"

class Apriori final : public FPAlgorithm {
private:
    void loadInitCandidates();
    bool loadNextCandidates(bool verbose = false);
    bool removeLessSupport(const unsigned int minSupport);
    std::vector<Pattern> candidateVec;

public:
    Apriori(DataLoader& dataLoader) : FPAlgorithm(dataLoader) { };
    void findFrequentItemsets(Pattern& frequentItemsets, const unsigned int minSupport,
                              bool verbose = false)
    override;
};

