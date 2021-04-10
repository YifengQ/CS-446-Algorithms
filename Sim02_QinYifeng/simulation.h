//
// Created by Yifeng Qin on 3/8/2019.
//

#ifndef SIM02_QINYIFENG_SIMULATION_H
#define SIM02_QINYIFENG_SIMULATION_H

#include <iostream>
#include <vector>
#include <pthread.h>
#include "config.h"
#include "PCB.h"
using namespace std;

class simulation {

public:
    simulation();

    void setVector(string, string, int);

    void popVector();
    void popFrontVector();
    void printMonitor();
    void printFile(config& x);

    void printVector();
    double simWait(double);

    double execute(double);

    //void *sim_thread(void* duration);

    string generateHexString();
    void run(PCB& PCB);

    const int start = 1;
    const int ready = 2;
    const int running = 3;
    const int wait = 4;
    const int exitProcess = 5; /// change due to exit() being a function

private:

    vector<string> code;
    vector<string> descriptor;
    vector<int> cycles;
    vector<string> toPrint;
    vector<PCB> state;
    unsigned int process;
    double elapsed_time;
};


#endif //SIM02_QINYIFENG_SIMULATION_H
