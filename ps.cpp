#include "ps.h"


void genNums(int *arr, int mean, int deviation){//fills array "arr" with random numbers using normal distribution
	std::default_random_engine generator;
	std::normal_distribution<double> distribution(mean,deviation);
	for(int i=0;i<50;i++){
		while(1){
			if(((arr[i] = distribution(generator)) > 0) && arr[i] < 11000)//make sure generated number is positive
				break;
		}
	}

}

results::results(){
	for(int i=0;i<50;i++)
		waitTime[i] = 0;
}

processhandler::processhandler(unsigned int memSize){
	currentPID = 0;
	maxCycles = 11000;
	//maxMem = 100;
	//totalMem = 0;
	totalCycles = 0;
	totalMemory = new int[memSize];
}

processhandler::~processhandler(){
	delete totalMemory;
}

void processhandler::addProcess(unsigned int n){//add process with cycles n
	process newProcess;
	newProcess.pid = currentPID;
	unsigned int newCycles = n;
	if(newCycles > maxCycles)//if newCycles is too large, scale it back to maxCycles
		newCycles = maxCycles;
	totalCycles += newCycles;
	newProcess.numcycles = newCycles;
	newProcess.origCycles = newCycles;
	processes.push_back (newProcess);
	currentPID++;

}



void processhandler::printProcesses(){//print all processes
	for(int i=0; i < processes.size(); i++){
		std::cout << "Process " << i << "- " << std::endl;
		std::cout << "\tPID: " << processes[i].pid;
		std::cout << "\tCycles: " << processes[i].numcycles << std::endl;
	}

}

void processhandler::printAverage(){//print average cycles and memory
	float averageCycles = totalCycles/currentPID;
	//float averageMem = totalMem/currentPID;
	std::cout << "Mean Cycles: " << averageCycles << std::endl;
	//std::cout << "Mean Memory: " << averageMem << std::endl;
}


ps::ps(){
	genNums(testcycles1,6000,1000);//set testcycles1 to have mean of 60000 and standard deviation of 1000
	genNums(testcycles2,6000,4000);
	genNums(testcycles3,3000,6000);
	for(int i = 0; i<50;i++){//set all processes in testcycles4 to 3000 with no standard deviation
		testcycles4[i] = 3000;
	}
}

results ps::runFIFO(int *tcycles){//runs first in first out process scheduler for one processor
	
	processhandler PH(100000);
	results R;//results structure
	unsigned int elapsedTime = 0;
	unsigned int numProcesses = 0;
	unsigned int penalty = 0;
	unsigned int pLeft = 50;
	process *p1 = NULL;//pointer to a process, acts as a processor
	
	while(pLeft>0){
		if(((elapsedTime % 50) == 0) && (numProcesses < 50)){//every 50 cycles, add process to waiting queue
			PH.addProcess(tcycles[numProcesses]);//add process from tcycles array
			numProcesses++;
		}


		if((!PH.processes.empty()) && (!p1)){//if p1 is not working on a process, take one off the waiting queue
			penalty+=10;//context switch penalty of 10 cycles
			p1 = new process;
			*p1 = PH.processes.front();//take process off front of waiting queue to satisfy FIFO
			PH.processes.pop_front();
		}



		if(p1){
			p1->numcycles--;//decrement numcycles from process that p1 is working on
			if(p1->numcycles==0){//if numcycles is 0, process is complete
				pLeft--;
				p1 = NULL;//set p1 to null so that it will pick up next waiting process
			}
		}

		for(int i=0; i<PH.processes.size(); i++){//update all wait times that are in waiting queue
			R.waitTime[PH.processes[i].pid]++;
		}
		elapsedTime++;
	}
	elapsedTime+=penalty;//add total penalty to elapsed time
	R.elapsedTime = elapsedTime;
	if(p1) delete p1;//free memory

	return R;
	
}


results ps::runFIFOmult(int *tcycles){//same as 1 processor FIFO but with processors p1-p4
	
	processhandler PH(100000);
	results R;
	unsigned int elapsedTime = 0;
	unsigned int numProcesses = 0;
	unsigned int penalty = 0;
	unsigned int pLeft = 50;
	process *p1 = NULL;
	process *p2 = NULL;
	process *p3 = NULL;
	process *p4 = NULL;
	
	while(pLeft>0){
		if(((elapsedTime % 50) == 0) && (numProcesses < 50)){
			PH.addProcess(tcycles[numProcesses]);
			numProcesses++;
		}

		if((!PH.processes.empty()) && (!p1)){
			penalty+=10;
			p1 = new process;
			*p1 = PH.processes.front();
			PH.processes.pop_front();
		}

		if((!PH.processes.empty()) && (!p2)){
			penalty+=10;
			p2 = new process;
			*p2 = PH.processes.front();
			PH.processes.pop_front();
		}

		if((!PH.processes.empty()) && (!p3)){
			penalty+=10;
			p3 = new process;
			*p3 = PH.processes.front();
			PH.processes.pop_front();
		}

		if((!PH.processes.empty()) && (!p4)){
			penalty+=10;
			p4 = new process;
			*p4 = PH.processes.front();
			PH.processes.pop_front();
		}

		if(p1){
			p1->numcycles--;
			if(p1->numcycles==0){
				pLeft--;
				p1 = NULL;
			}
		}

		if(p2){
			p2->numcycles--;
			if(p2->numcycles==0){
				pLeft--;
				p2 = NULL;
			}
		}

		if(p3){
			p3->numcycles--;
			if(p3->numcycles==0){
				pLeft--;
				p3 = NULL;
			}
		}

		if(p4){
			p4->numcycles--;
			if(p4->numcycles==0){
				pLeft--;
				p4 = NULL;
			}
		}
		for(int i=0; i<PH.processes.size(); i++){//update all wait times that are in waiting queue
			R.waitTime[PH.processes[i].pid]++;
		}
		elapsedTime++;
	}
	elapsedTime+=penalty;
	R.elapsedTime = elapsedTime;
	if(p1) delete p1;
	if(p2) delete p1;
	if(p3) delete p1;
	if(p4) delete p1;
	return R;
}


void ps::testFIFO(int *testcycles){
	std::cout << "FIFO (1 PROCESSOR)" << std::endl;
	printResults(testcycles,runFIFO(testcycles));
	std::cout << std::endl;
	std::cout << "FIFO (4 PROCESSORS)" << std::endl;
	printResults(testcycles,runFIFOmult(testcycles));
	std::cout << std::endl;
}

void ps::test(){
	testFIFO(testcycles1);
}

void ps::printResults(int *testcycles, results R){
	int avgWaitTime = 0;
	std::cout << "Process\tCycles\tWait Time" << std::endl;
	for(int i=0; i < 50; i++){
		std::cout << "p" << i << ":\t" << testcycles[i] << "\t" << R.waitTime[i] << std::endl;
		avgWaitTime+=R.waitTime[i];
	}
	avgWaitTime = avgWaitTime/50;
	std::cout << "Total Elapsed Time:\t" << R.elapsedTime << std::endl;
	std::cout << "Average Wait Time:\t" << avgWaitTime << std::endl;
}
