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

void genMem(int *arr, unsigned int maxSize){
	std::default_random_engine generator;
	double sum = 0;
	double newsum = 0;
	double a[50];
	for(int i=0;i<50;i++){
		a[i] = generator() % 100;
		sum+=a[i];
	}

	for(int i = 0; i<50;i++){
		a[i] = a[i]/sum;
		a[i] *= maxSize;
		arr[i] = a[i];
		newsum += arr[i];
		std::cout << arr[i] << std::endl;
	}
	std::cout << "new sum is " << newsum/1000000 << "mb" << std::endl;

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
	for(int i=0;i<memSize;i++){
		totalMemory[i] = -1;
	}
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
	genMem(mempattern, MAXMEMSIZE);
	for(int i = 0; i<50;i++){//set all processes in testcycles4 to 3000 with no standard deviation
		testcycles4[i] = 3000;
	}
}


results ps::runFIFOsystem(int *tcycles, int *mempattern, int numProcessors){
	
	processhandler PH(100000);
	results R;
	unsigned int elapsedTime = 0;
	unsigned int numProcesses = 0;
	unsigned int penalty = 0;
	unsigned int pLeft = 50;
	process *p[numProcessors];
	char *m[numProcessors];

	for(int i = 0; i<numProcessors;i++){
		p[i] = NULL;
		m[i] = NULL;
	}

	while(pLeft>0){

		if(((elapsedTime % 50) == 0) && (numProcesses < 50)){
			PH.addProcess(tcycles[numProcesses]);
			numProcesses++;
		}

		for(int i = 0; i<numProcessors;i++){
			if(!PH.processes.empty() && (!p[i])){
				penalty+=10;
				p[i] = new process;
				*p[i] = PH.processes.front();
				PH.processes.pop_front();
				//allocation
				m[i] = (char*)malloc(mempattern[p[i]->pid]);
			}

			if(p[i]){
				p[i]->numcycles--;
				if(p[i]->numcycles==0){
					pLeft--;
					p[i] = NULL;
					free(m[i]);
				}
			}
		}


		for(int i=0; i<PH.processes.size(); i++){//update all wait times that are in waiting queue
			R.waitTime[PH.processes[i].pid]++;
		}
		elapsedTime++;
	}
	elapsedTime+=penalty;
	R.elapsedTime = elapsedTime;
	for(int i=0; i<numProcessors;i++){
		if(p[i]) delete p[i];
	}

	return R;
}

results ps::runFIFO(int *tcycles, int numProcessors){
	
	processhandler PH(100000);
	results R;
	unsigned int elapsedTime = 0;
	unsigned int numProcesses = 0;
	unsigned int penalty = 0;
	unsigned int pLeft = 50;
	process *p[numProcessors];
	char *totalMemory = (char*)malloc(MAXMEMSIZE);

	for(int i = 0; i<numProcessors;i++){
		p[i] = NULL;
	}

	while(pLeft>0){

		if(((elapsedTime % 50) == 0) && (numProcesses < 50)){
			PH.addProcess(tcycles[numProcesses]);
			numProcesses++;
		}

		for(int i = 0; i<numProcessors;i++){
			if(!PH.processes.empty() && (!p[i])){
				penalty+=10;
				p[i] = new process;
				*p[i] = PH.processes.front();
				PH.processes.pop_front();
			}

			if(p[i]){
				p[i]->numcycles--;
				if(p[i]->numcycles==0){
					pLeft--;
					p[i] = NULL;
				}
			}
		}


		for(int i=0; i<PH.processes.size(); i++){//update all wait times that are in waiting queue
			R.waitTime[PH.processes[i].pid]++;
		}
		elapsedTime++;
	}
	elapsedTime+=penalty;
	R.elapsedTime = elapsedTime;
	for(int i=0; i<numProcessors;i++){
		if(p[i]) delete p[i];
	}

	return R;
}


void ps::testFIFO(int *testcycles){
	std::cout << "FIFO (1 PROCESSOR)" << std::endl;
	printResults(testcycles,runFIFOsystem(testcycles,mempattern,1));
	std::cout << std::endl;
	std::cout << "FIFO (4 PROCESSORS)" << std::endl;
	printResults(testcycles,runFIFOsystem(testcycles,mempattern,4));
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
