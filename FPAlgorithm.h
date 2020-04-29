#pragma once
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include "DataLoader/DataLoader.h"
#include <ostream>

struct SetHash {
    std::size_t operator()(const std::unordered_set<unsigned int>& k) const {
        int sum = 0;
        for (auto item : k) {
            sum += std::hash<unsigned>()(item);
        }
        return sum;
    }
};

// {1,2}: 23  // 集合: 出现次数
typedef std::unordered_map<std::unordered_set<unsigned int>, unsigned int, SetHash> Pattern;
// <{1,2}, 4, 0.6>  // <条件, 结果, 置信度>
//typedef std::vector<std::tuple<std::unordered_set<unsigned int>, unsigned int, double>> Rule;
//typedef std::vector<std::tuple<std::unordered_set<std::string>, std::string, double>> MeaningRule;
typedef std::vector<std::tuple<std::unordered_set<unsigned int>, std::unordered_set<unsigned int>, double>> Rule;
typedef std::vector<std::tuple<std::unordered_set<std::string>, std::unordered_set<std::string>, double>> MeaningRule;

extern std::ostream& operator<<(std::ostream& os, const Pattern& patterns);
extern std::ostream& operator<<(std::ostream& os, const Rule& rules);
extern std::ostream& operator<<(std::ostream& os, const MeaningRule& rules);

extern void saveCandidates(const std::string& filename, const Pattern& candidates);

class FPAlgorithm {
protected:
    DataLoader& dataLoader;
public:
    explicit FPAlgorithm(DataLoader& dataLoader)
        : dataLoader(dataLoader) {
    }
    static void generateRules(const Pattern& patterns, Rule& rules, const double confidence);
    static void generateRules(const Pattern& patterns, MeaningRule& rules, 
                              const std::unordered_map<unsigned int, std::string>& map, const double confidence);
    virtual ~FPAlgorithm() = default;
    virtual void findFrequentItemsets(Pattern& frequentItemsets,
                                      const unsigned int minSupport, 
                                      bool verbose = false) = 0;
};
