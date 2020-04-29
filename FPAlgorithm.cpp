#include "FPAlgorithm.h"
#include <fstream>
#include <iostream>
#include <algorithm>


std::ostream& operator<<(std::ostream& os, const Pattern& patterns) {
    if (patterns.empty()) {
        os << "{}";
        return os;
    }
    os << '{';
    for (const auto& item : patterns) {
        os << "<{";
        for (auto tmp : item.first) {
            os << tmp;
            os << ", ";
        }
        os << "\b\b}: " << item.second << ">, ";
    }
    os << "\b\b}";
    return os;
}

std::ostream& operator<<(std::ostream& os, const Rule& rules) {
    if (rules.empty()) {
        os << "{}";
        return os;
    }
    os << '{' << std::endl;
    for (const auto& item : rules) {
        os << "<{";
        for (auto tmp : std::get<0>(item)) {
            os << tmp;
            os << ", ";
        }
        os << "\b\b} -> {";
        for (auto tmp : std::get<1>(item)) {
            os << tmp;
            os << ", ";
        }
        os << "\b\b} : " << std::get<2>(item) << ">, " << std::endl;
    }
    os << '}' << std::endl;
    return os;
}

std::ostream& operator<<(std::ostream& os, const MeaningRule& rules) {
    if (rules.empty()) {
        os << "{}";
        return os;
    }
    os << '{' << std::endl;
    for (const auto& item : rules) {
        os << "<{ ";
        for (const auto& tmp : std::get<0>(item)) {
            os << tmp;
            os << ", ";
        }
        os << "\b\b } -> { ";
        for (const auto& tmp : std::get<1>(item)) {
            os << tmp;
            os << ", ";
        }
        os << "\b\b } : " << std::get<2>(item) << ">, " << std::endl;
    }
    os << '}' << std::endl;
    return os;
}

void saveCandidates(const std::string& filename, const Pattern& candidates) {
    //std::cout << filename << std::endl;
    std::ofstream outfile(filename);
    for (const auto& item : candidates) {
        const auto setSize = item.first.size();
        unsigned int i = 0;
        for (auto tmp : item.first) {
            outfile << tmp;
            if (i++ < setSize - 1) {
                outfile << ",";
            }
        }
        outfile << ":" << item.second << '\n';
    }
    outfile.close();
}

void FPAlgorithm::generateRules(const Pattern& patterns, Rule& rules, const double confidence) {
    rules.clear();
    for (const auto& patternFartherPair : patterns) {
        for (const auto& patternSonPair : patterns) {
            if (patternFartherPair.first.size() <= patternSonPair.first.size()) {
                continue;
            }
            auto fartherSetCopy(patternFartherPair.first);
            auto subsetFlag = true;
            for (auto item : patternSonPair.first) {
                if (fartherSetCopy.find(item) == fartherSetCopy.end()) {
                    subsetFlag = false;
                    break;
                } else {
                    fartherSetCopy.erase(item);
                }
            }
            if (subsetFlag) {
                auto currentConfidence = double(patternFartherPair.second) / double(patternSonPair.second);
                rules.emplace_back(std::make_tuple(patternSonPair.first, fartherSetCopy, currentConfidence));
            }
        }
    }
    std::sort(rules.begin(), rules.end(),
              [](const std::tuple<std::unordered_set<unsigned int>, std::unordered_set<unsigned int>, double>& a,
                 const std::tuple<std::unordered_set<unsigned int>, std::unordered_set<unsigned int>, double>& b) {
                     return std::get<2>(a) > std::get<2>(b);
              });
}

void FPAlgorithm::generateRules(const Pattern& patterns, MeaningRule& rules,
    const std::unordered_map<unsigned, std::string>& map, const double confidence) {
    Rule meaninglessRules;
    generateRules(patterns, meaninglessRules, confidence);
    for (auto itemTuple : meaninglessRules) {
        std::unordered_set<std::string> desSet;
        for (auto des : std::get<1>(itemTuple)) {
            if (map.find(des) != map.end()) {
                desSet.emplace(map.at(des));
            }
        }
        std::unordered_set<std::string> preSet;
        for (auto item : std::get<0>(itemTuple)) {
            if (map.find(item) != map.end()) {
                preSet.emplace(map.at(item));
            }
        }
        rules.emplace_back(std::make_tuple(preSet, desSet, std::get<2>(itemTuple)));
    }
}
