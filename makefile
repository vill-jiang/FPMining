FPMining : obj/FPMiningMain.o obj/FPAlgorithm.o obj/DataLoader.o obj/Apriori.o obj/FPGrowth.o obj/FPTree.o
	g++ -std=c++11 -o bin/FPMining obj/FPMiningMain.o obj/FPAlgorithm.o obj/DataLoader.o obj/Apriori.o obj/FPGrowth.o obj/FPTree.o -static

obj/FPMiningMain.o : FPMining/FPMining.cpp FPMining/cxxopts.hpp
	g++ -O3 -std=c++11 -o obj/FPMiningMain.o -c FPMining/FPMining.cpp
obj/FPAlgorithm.o : FPAlgorithm.cpp FPAlgorithm.h
	g++ -O3 -std=c++11 -o obj/FPAlgorithm.o -c FPAlgorithm.cpp
obj/DataLoader.o : DataLoader/DataLoader.cpp DataLoader/DataLoader.h
	g++ -O3 -std=c++11 -o obj/DataLoader.o -c DataLoader/DataLoader.cpp
obj/Apriori.o : Apriori/Apriori.cpp Apriori/Apriori.h
	g++ -O3 -std=c++11 -o obj/Apriori.o -c Apriori/Apriori.cpp
obj/FPGrowth.o : FPGrowth/FPGrowth.cpp FPGrowth/FPGrowth.h
	g++ -O3 -std=c++11 -o obj/FPGrowth.o -c FPGrowth/FPGrowth.cpp
obj/FPTree.o : FPGrowth/FPTree.cpp FPGrowth/FPTree.h
	g++ -O3 -std=c++11 -o obj/FPTree.o -c FPGrowth/FPTree.cpp

.PHONY : clean
clean :