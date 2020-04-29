// Apriori.cpp : 此文件包含 "main" 函数。程序执行将在此处开始并结束。
//

#include "../DataLoader/DataLoader.h"
#include "Apriori.h"

int main()
{
    DataLoader dl("E:\\NJU\\NJU2019-2020\\下04_数据挖掘导论\\作业\\FPMining\\UNIX_usage.set");  // ../DataLoader/test.txt
    Apriori apriori(dl);
    Pattern frequentItemsets;
    apriori.findFrequentItemsets(frequentItemsets, 20);
}