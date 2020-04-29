// FPGrowth.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include <iostream>

#include "../DataLoader/DataLoader.h"
#include "FPGrowth.h"

int main()
{
    DataLoader dl("../DataLoader/test.txt");  // ../DataLoader/test.txt
    FPGrowth fpGrowth(dl);
    Pattern frequentItemsets;
    fpGrowth.findFrequentItemsets(frequentItemsets, 2);
    std::cout << frequentItemsets << std::endl;
}
