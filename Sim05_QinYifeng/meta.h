//
// Created by Yifen on 2/16/2019.
//

#ifndef UNTITLED_META_H
#define UNTITLED_META_H


#include <iostream>
#include <vector>
#include "config.h"
#include "simulation.h"

using namespace std;

class meta {

public:

    meta();
    void readMeta(config& x, simulation& sim);
    bool checkStart(string, simulation);
    bool saveData(string , config& x, simulation& sim);
    bool setApp(string);
    int setProcess(string, int,  config& x);
    int setInput(string, int,  config& x);
    int setOutput(string, int,  config& x);
    int setMemory(string, int,  config& x);
    bool checkFinish(string);
    bool ifFinish(string);
    void printMeta();
    void printToFile(config& x);
    string junk(string);
    bool ifError();

private:

    vector<string> meta_data;
    vector<int> cycles;
    int indexError;
    int index;


};


#endif //UNTITLED_META_H
