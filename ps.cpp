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
		//std::cout << arr[i] << std::endl;
	}
	//std::cout << "new sum is " << newsum/1000000 << "mb" << std::endl;

}

memHandler::memHandler(unsigned int memSize){
	memory = (char*)malloc(memSize);
	for(int i=0;i<memSize;i++){
		memory[i]='X';//x will symbolize and empty space
	}
	totalMemSize = memSize;
	current = 0;
}

void memHandler::printMem(){
	for(int i=0;i<totalMemSize;i++){
		std::cout << i << "\t" << (int)memory[i] << std::endl;
	}
}

bool memHandler::new_malloc(process p){//malloc using next best fit
	unsigned int max = current + p.memory;//maximum boundary to check for free memory
	int i = current;//start at current
	int newstart = current;
	int counter = 0;//counts number of memory spaces checked
	do{
		if(i == max){
		//	std::cout << "current: " << current << "\tnewstart: " << newstart << "\ti: " << i << std::endl;
			for(int n = newstart; n<i;n++){
				memory[n] = p.pid;
			}
			current = max;
			return 1;
		}
		if(max >= totalMemSize){
			i = newstart = 0;
			max = p.memory;
		}
		if(memory[i] != 'X'){//if memory space is not empty
			int newmax = (max + p.memory);//find new maximum boundary
			if(newmax < totalMemSize){//if new maximum boundary is in range of total memory size, change i to current max
				i = newstart = max;
				max = newmax;//change max to newly calculated max
			}
			else{//if new maximum boundary is out of range of total memory size, start search at 0
				i = newstart = 0;
				max = p.memory;
			}
		}
		else
			i++;
		counter++;
	}while(counter<totalMemSize);//breaks when all memory has been checked
	return 0;//return false if failed to find sufficient memory
}

void memHandler::new_free(unsigned int pid){//free all memory that is attached to PID
	for(int i=0; i<totalMemSize;i++){
		if(memory[i] == pid){
			memory[i] = 'X';
		}
	}
}

int memHandler::get_mem_proc(unsigned int pid){
	int count = 0;
	for(int i=0;i<totalMemSize;i++){
		if(memory[i] == pid){
			count++;
		}
	}
	return count;
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

void processhandler::addProcess(unsigned int n, unsigned int mem = 0){//add process with cycles n
	process newProcess;
	newProcess.pid = currentPID;
	unsigned int newCycles = n;
	if(newCycles > maxCycles)//if newCycles is too large, scale it back to maxCycles
		newCycles = maxCycles;
	totalCycles += newCycles;
	newProcess.numcycles = newCycles;
	newProcess.origCycles = newCycles;
	newProcess.memory = mem;
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
				std::cout << "allocating: " << p[i]->pid<< std::endl;
				m[i] = (char*)malloc(mempattern[p[i]->pid]);
			}

			if(p[i]){
				p[i]->numcycles--;
				if(p[i]->numcycles==0){
					std::cout << "freed: " << p[i]->pid<< std::endl;
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

results ps::runFIFO(int *tcycles, int *tmem, unsigned int total_mem, int numProcessors){
	
	processhandler PH(100000);
	//memHandler MH(400000);
	memHandler MH(total_mem);
	results R;
	unsigned int elapsedTime = 0;
	unsigned int numProcesses = 0;
	unsigned int penalty = 0;
	unsigned int pLeft = 50;
	process *p[numProcessors];
	char *totalMemory = (char*)malloc(MAXMEMSIZE);
	int memStart;
	int memEnd;


	for(int i = 0; i<numProcessors;i++){
		p[i] = NULL;
	}

	while(pLeft>0){

		if(((elapsedTime % 50) == 0) && (numProcesses < 50)){
			PH.addProcess(tcycles[numProcesses], tmem[numProcesses]);
			numProcesses++;
		}

		for(int i = 0; i<numProcessors;i++){//problem is in here
			if(!p[i]){
				if(!MH.waitList.empty() && MH.new_malloc(MH.waitList.front())){
					std::cout << "allocating from wait queue: " << MH.waitList.front().pid << std::endl;
					p[i] = new process;
					*p[i] = MH.waitList.front();
					MH.waitList.pop_front();
				}
				else if(!PH.processes.empty()){
					penalty+=10;
					if(MH.new_malloc(PH.processes.front())){
						std::cout << "allocating: " << PH.processes.front().pid << std::endl;
						p[i] = new process;
						*p[i] = PH.processes.front();
					}
					else{
						std::cout << "not enough memory, pushed to wait queue: " << PH.processes.front().pid << std::endl;
						MH.waitList.push_back(PH.processes.front());
					}
					PH.processes.pop_front();
				}
			}

			if(p[i]){
				p[i]->numcycles--;

				if(p[i]->numcycles==0){
					std::cout << "freed: " << p[i]->pid<< std::endl;
					pLeft--;
					MH.new_free(p[i]->pid);
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
	//MH.printMem();
	return R;
}


void ps::testFIFO(int *testcycles){
	clock_t start, diff;
	int t1,t2,t3,t4;

	std::cout << "System Malloc - 4 Processors" << std::endl;
	start = clock();
	runFIFOsystem(testcycles,mempattern,4);
	diff = clock() - start;
	t1 = diff * 1000 / CLOCKS_PER_SEC;
	std::cout << std::endl;

	std::cout << "New Malloc, perfect fit - 4 Processors" << std::endl;
	start = clock();
	runFIFO(testcycles,mempattern, MAXMEMSIZE , 4);
	diff = clock() - start;
	t2 = diff * 1000 / CLOCKS_PER_SEC;
	std::cout << std::endl;

	std::cout << "New Malloc, 50% available memory - 4 Processors" << std::endl;
	start = clock();
	runFIFO(testcycles,mempattern, MAXMEMSIZE/2 , 4);
	diff = clock() - start;
	t3 = diff * 1000 / CLOCKS_PER_SEC;
	std::cout << std::endl;

	std::cout << "New Malloc, 10% available memory - 4 Processors" << std::endl;
	start = clock();
	runFIFO(testcycles,mempattern, MAXMEMSIZE/10 , 4);
	diff = clock() - start;
	t4 = diff * 1000 / CLOCKS_PER_SEC;
	std::cout << std::endl;

	std::cout << "System Run Times:" << std::endl;
	std::cout << "System Malloc:\t" << t1 << "ms"<< std::endl;
	std::cout << "New Malloc, perfect fit:\t" << t2 << "ms" << std::endl;
	std::cout << "New Malloc, 50% available memory:\t" << t3 << "ms" << std::endl;
	std::cout << "New Malloc, 10% available memory:\t" << t4 << "ms" <<std::endl;

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
