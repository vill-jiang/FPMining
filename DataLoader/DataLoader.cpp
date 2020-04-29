#include "DataLoader.h"

#include <fstream>
#include <iostream>

DataLoader::DataLoader(const std::string& filename) {
    if (readFromFile(filename) == -1) {
        std::cout << "File (" << filename << ") don't exist.";
        exit(-1);
    }
}

bool DataLoader::loadMap(std::unordered_map<unsigned, std::string>& map, const std::string& mapFilename) {
    std::ifstream in(mapFilename);
    if (in.is_open()) {
        std::string line;
        std::getline(in, line);
        while (!line.empty()) {
            const auto j = line.find(DataLoader::DELIMITER);
            if (j == std::string::npos) {
                continue;
            } else {
                map.emplace(std::stoul(line.substr(0, j)), line.substr(j + 1));
            }
            std::getline(in, line);
        }
        in.close();
        return true;
    } else {
        return false;
    }
}

void DataLoader::clear() {
    this->index = 0;
    this->transactions.clear();
}

int DataLoader::readFromFile(const std::string& filename) {
    std::ifstream in(filename);
    if (in.is_open()) {
        const auto before = this->transactions.size();
        std::string line;
        std::getline(in, line);
        while (!line.empty()) {
            std::unordered_set<unsigned int> tmpSet;
            size_t i = 0;
            size_t j = 0;
            while(i < line.length()) {
                j = line.find(DataLoader::DELIMITER, i);
                if (j == std::string::npos) {
                    break;
                } else {
                    tmpSet.emplace(std::stoul(line.substr(i, j - i)));
                    i = j + 1;
                }
            }
            if (!tmpSet.empty()) {
                this->transactions.emplace_back(tmpSet);
            }
            std::getline(in, line);
        }
        in.close();
        return this->transactions.size() - before;
    } else {
        return -1;
    }
}

void DataLoader::reset() {
    this->index = 0;
}

const std::unordered_set<unsigned int>& DataLoader::getNext(){
    if (this->hasNext()) {
        return this->transactions[this->index++];
    } else {
        return this->transactions.back();
    }
}

bool DataLoader::hasNext() const {
    return this->index < this->transactions.size();
}
