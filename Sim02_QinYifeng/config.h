//
// Created by yifengq on 2/11/19.
//

#ifndef UNTITLED_CONFIG_H
#define UNTITLED_CONFIG_H

#include <iostream>

using namespace std;

class config {

    friend class meta;

public:

    config();
    bool setData(string);
    void log_config_monitor();
    void log_config_file();
    int getMonitor();
    int getProcessor();
    int getScanner();
    int getHD();
    int getKeyboard();
    int getMemory();
    int getProjector();
    string getPath();
    string getMetaPath();
    int getlogto();
    string junk(string);

private:

    int file;
    int monitor;
    int processor;
    int scanner;
    int hard_drive;
    int keyboard;
    int memory;
    int projector;
    int logto;
    int systemMemory;
    string conf_path;
    string file_path;
    string meta_path;
    string error;

};


#endif //UNTITLED_CONFIG_H
