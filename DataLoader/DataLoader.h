#pragma once
#include <string>
#include <unordered_map>
#include <vector>
#include <unordered_set>

/*
 * The class can read .set file generated by data_process.py only.
 * The .set file is organized as follows:
 * 0,1,2,3,4,5,6,
 * 2,4,5,7,8,
 * 9,10,
 * 9,11,12,13,14,15,
 * 16,9,
 * something important: the data is unsigned int (start with 0).
 */
class DataLoader {
private:
    static const char DELIMITER = ',';
    std::vector<std::unordered_set<unsigned int>> transactions;
    unsigned int index = 0;
public:
    DataLoader() = default;
    DataLoader(const std::string& filename);
    static bool loadMap(std::unordered_map<unsigned int, std::string>& map, const std::string& mapFilename);
    void clear();
    int readFromFile(const std::string& filename);
    const std::unordered_set<unsigned int>& getNext();
    bool hasNext() const;
    void reset();
};
