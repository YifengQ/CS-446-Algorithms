//
// Created by Yifeng Qin on 3/8/2019.
//

#ifndef SIM02_QINYIFENG_SIMULATION_H
#define SIM02_QINYIFENG_SIMULATION_H

#include <iostream>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include "config.h"
#include "PCB.h"
using namespace std;

class simulation {

public:
    simulation();
    simulation(config& x);

    void setVector(string, string, int);

    void popVector();
    void popFrontVector();
    void printMonitor();
    void printFile(config& x);

    void printVector();
    double simWait(double);

    double execute(double, sem_t &, pthread_mutex_t &);

    void init();
    void destructor();
    //void *sim_thread(void* duration);

    string generateHexString();///for random

    string setHexMemory();


    void run(PCB& PCB);

    void setProjIndex();
    void setHDIndex();

    void sortSched();

    void appendVector(vector<string> & v, const vector<string> & w);

    void appendCycles(vector<int> & v, const vector<int> & w);
    const int start = 1;
    const int ready = 2;
    const int running = 3;
    const int wait = 4;
    const int exitProcess = 5; /// change due to exit() being a function

private:

    vector<string> code;
    vector<string> descriptor;
    vector<string> sorted;
    vector<int> sortedProcess;
    vector<int> sortedCycles;
    vector<int> cycles;
    vector<string> toPrint;
    vector<PCB> state;
    string sched;
    unsigned int process;
    double elapsed_time;
    unsigned int numProj;
    unsigned int numHD;
    unsigned int idxProj;
    unsigned int idxHD;
    unsigned int blockSize;
    unsigned int memorySize;
    unsigned int currMem;
};


#endif //SIM02_QINYIFENG_SIMULATION_H
