#include <chrono>
#include <iostream>
#include "cxxopts.hpp"
#include "../DataLoader/DataLoader.h"
#include "../FPGrowth/FPGrowth.h"

void candidatesCamp(const Pattern& a, const Pattern& b);
void removePatterns(Pattern& p, const unsigned int top);

int main(int argc, char** argv) {
    cxxopts::Options options("FPMining", "Apriori & FPGrowth implement with c++. Author: Jiang HuiYu.");
    options.add_options()
        ("i,input", "Input file name", cxxopts::value<std::string>()->default_value("../DataLoader/test.txt"))
        ("m,map", "Input map file name (default: input + '.dict')")
        ("o,output", "Output file name prefix", cxxopts::value<std::string>())
        ("a,algorithm", "Using algorithm: 'fpgrowth', 'apriori', 'all'", cxxopts::value<std::string>()->default_value("all"))
        ("s,support", "Minimal support", cxxopts::value<unsigned int>()->default_value("2"))
        ("c,confidence", "Minimal confidence", cxxopts::value<double>()->default_value("0.2"))
        ("t,top-k", "Output top-k association rule", cxxopts::value<unsigned int>()->default_value("100"))
        //("r,remove", "Remove top-k frequent patterns", cxxopts::value<unsigned int>()->default_value("0"))
        ("d,debug", "Enable debugging", cxxopts::value<bool>()->default_value("false"))
        ("v,verbose", "Verbose output", cxxopts::value<bool>()->default_value("false"))
        ("p,compare", "Compare two results", cxxopts::value<bool>()->default_value("false"))
        ("h,help", "Print usage")
    ;
    auto result = options.parse(argc, argv);
    if (result.count("help")) {
        std::cout << options.help() << std::endl;
        return 0;
    }
    auto debug = result["debug"].as<bool>();
    auto verbose = result["verbose"].as<bool>();
    auto compare = result["compare"].as<bool>();
    auto file = result["input"].as<std::string>();
    auto algorithm = result["algorithm"].as<std::string>();
    auto minSupport = result["support"].as<unsigned int>();
    auto minConfidence = result["confidence"].as<double>();
    auto top = result["top-k"].as<unsigned int>();
    //auto remove = result["remove"].as<unsigned int>();
    std::string output;
    if (result.count("output")) {
        output = result["output"].as<std::string>();
    }
    std::string mapFile;
    if (result.count("map")) {
        mapFile = result["map"].as<std::string>();
    } else {
        mapFile = file + ".dict";
    }


    DataLoader dl(file);

    std::chrono::system_clock::time_point startWith;
    std::chrono::duration<double> diff;
    Pattern fpGrowthItemsets;
    if (algorithm == "fpgrowth" || algorithm == "all") {
        startWith = std::chrono::system_clock::now();
        FPGrowth fpGrowth(dl);
        fpGrowth.findFrequentItemsets(fpGrowthItemsets, minSupport, verbose);
        diff = std::chrono::system_clock::now() - startWith;
        std::cout << "FPGrowth time consuming: " << diff.count() << " s, get "
                  << fpGrowthItemsets.size() << " itemsets" << std::endl << std::endl;

        if (!output.empty()) {
            saveCandidates(output + "fpgrowth.itemsets", fpGrowthItemsets);
            std::cout << std::endl << "FPGrowth results save to: " << output + "fpgrowth.itemsets" << std::endl << std::endl;;
        }
        // remove top-k frequent pattern
        //removePatterns(fpGrowthItemsets, remove);
        if (debug) {
            std::cout << std::endl << "FPGrowth results: " << std::endl << fpGrowthItemsets << std::endl << std::endl;
        }
    }

    Pattern aprioriItemsets;
    if (algorithm == "apriori" || algorithm == "all") {
        startWith = std::chrono::system_clock::now();
        Apriori apriori(dl);
        apriori.findFrequentItemsets(aprioriItemsets, minSupport, verbose);
        diff = std::chrono::system_clock::now() - startWith;
        std::cout << "Apriori time consuming: " << diff.count() << " s, get "
                  << aprioriItemsets.size() << " itemsets" << std::endl << std::endl;

        if (!output.empty()) {
            saveCandidates(output + "apriori.itemsets", aprioriItemsets);
            std::cout << std::endl << "Apriori results save to: " << output + "apriori.itemsets" << std::endl << std::endl;;
        }
        // remove top-k frequent pattern
        //removePatterns(aprioriItemsets, remove);
        if (debug) {
            std::cout << std::endl << "Apriori results: " << std::endl << aprioriItemsets << std::endl << std::endl;
        }
    }

    // compare two itemsets
    if (algorithm == "all" && compare) {
        candidatesCamp(aprioriItemsets, fpGrowthItemsets);
    }

    // print top-k rules
    if (top > 0) {
        MeaningRule rules;
        std::unordered_map<unsigned int, std::string> map;
        DataLoader::loadMap(map, mapFile);
        if (algorithm == "apriori") {
            FPAlgorithm::generateRules(aprioriItemsets, rules, map, minConfidence);
        } else {
            FPAlgorithm::generateRules(fpGrowthItemsets, rules, map, minConfidence);
        }
        const auto beforeSize = rules.size();
        if (rules.size() > top) {
            rules.erase(rules.begin() + top, rules.end());
        }
        std::cout << "Generate " << beforeSize << " rules, show " << rules.size() << " rules." << std::endl;
        std::cout << rules;
    }

    return 0;
}

void candidatesCamp(const Pattern& a, const Pattern& b) {
    if (a.size() != b.size()) {
        std::cout << std::endl << "Size not equal: " << a.size() << " != " << b.size() << std::endl;
    }
    Pattern supportNotEqual;
    Pattern diff;
    for (const auto& item : a) {
        if (b.find(item.first) != b.end()) {
            if (b.at(item.first) != item.second) {
                supportNotEqual.emplace(item.first, item.second);
            }
        } else {
            diff.emplace(item.first, item.second);
        }
    }
    for (const auto& item : b) {
        if (a.find(item.first) == a.end()) {
            diff.emplace(item.first, item.second);
        }
    }
    if (diff.empty() && supportNotEqual.empty()) {
        std::cout << std::endl << "Two results strictly equal!" << std::endl;
    }
    if (!diff.empty()) {
        std::cout << std::endl << "Not exist in another result: " << diff.size() << std::endl << diff << std::endl;
    }
    if (!supportNotEqual.empty()) {
        std::cout << std::endl << "Not equal support in result: " << supportNotEqual.size() << std::endl << supportNotEqual << std::endl;
    }
}

void removePatterns(Pattern& p, const unsigned int top) {
    if (p.size() > top && top > 0) {
        std::vector<std::unordered_set<unsigned int>> patternVec;
        patternVec.reserve(p.size());
        for (const auto& itemSet : p) {
            patternVec.emplace_back(itemSet.first);
        }
        std::sort(patternVec.begin(), patternVec.end(),
                  [p](const std::unordered_set<unsigned int>& a, const std::unordered_set<unsigned int>& b) {
                         return p.at(a) > p.at(b);
                  });
        for (unsigned int i = 0; i < top; i++) {
            p.erase(patternVec[i]);
        }
    }
}
