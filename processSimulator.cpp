/*
	Must be compiled with ISO C++ standard C++0x

	Compilation example:
	g++ -std=c++0x processSimulator.cpp ps.cpp
*/
#include "ps.h"

int main(){
	ps Psim;
	//Psim.test();
	int start1, start2, start3;
	memHandler mh(1000);
	start1 = mh.new_malloc(300,3);
	start2 = mh.new_malloc(300,5);
	std::cout << "start1: " << start1 << std::endl;
	std::cout << "start2: " << start2 << std::endl;
	mh.new_free(3);
	start3 = mh.new_malloc(300,7);
	start3 = mh.new_malloc(400,8);
	std::cout << "start3: " << start3 << std::endl;

	//start3 = mh.new_malloc(300,10);
	//start3 = mh.new_malloc(100,9);
	mh.printMem();
	return 0;
}
