//
// Created by Yifeng Qin on 3/8/2019.
//
#include <iostream>
#include <chrono>
#include <ctime>
#include <time.h>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>

#include "simulation.h"

using namespace std;

/**
 * @breif Default constructor
 */
simulation::simulation() {

    process = 0;
    elapsed_time = 0;
}

/**
 * @breif This is a wait function that takes in the ms and waits. It takes
 * the start time and waits until the end time = the ms needed.
 * @param time
 * @return returns how long it waited so it can be added to a running total of the time.
 */
double simulation::simWait(double time) {

    bool sleep = true;
    auto start = chrono::high_resolution_clock::now(); /// gets start time
    while(sleep){
        auto now = chrono::high_resolution_clock::now(); /// keeps getting the clock
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - start); ///checks elapsed time
        if ( elapsed.count() > time ) ///if elapsed time = duration the exit
            sleep = false;
    }

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();

}

/**
 * @brief Will erase the last element of the vector. I takes the data from meta file
 * so every new line it pops the last element because it saved it twice.
 */
void simulation::popVector() {

    code.pop_back();
    descriptor.pop_back();
    cycles.pop_back();
}

/**
 * @brief Takes the data from the meta and config files and saves them into 3 vectors containing the
 * metadata code, metadata descriptor, and total ms needed.
 * @param str
 * @param str2
 * @param x
 */
void simulation::setVector(string str, string str2, int x) {

    code.push_back(str);
    descriptor.push_back(str2);
    cycles.push_back(x);
}

/**
 * @breif erases the first element in the vector because it contains the start
 */
void simulation::popFrontVector() {
    code.erase(code.begin());
    descriptor.erase(descriptor.begin());
    cycles.erase(cycles.begin());
}

/**
 * @breif It goes through the vectors that were saved from the metadata. With that it goes through
 * each element and checks the descriptor. When it finds out which descriptor it is, it will set PCB state
 * then record the start time,use the wait function to wait the needed ms, and then record the end time. It saves to
 * another vector (toPrint) so it can print easily to file and monitor.
 * @param PCB
 */
void simulation::run(PCB& PCB) {

    string begin ("begin");
    string run("run");
    string allocate("allocate");
    string monitor("monitor");
    string hard_drive("harddrive");
    string scanner ("scanner");
    string projector("projector");
    string finish("finish");
    string block("block");
    string keyboard("keyboard");
    string startStr ("start");

    PCB.setState(start); /// S{begin} gets start because no errors in meta
    state.push_back(PCB);
    toPrint.push_back(to_string(elapsed_time) + " - Simulator program starting" + "\n"); ///stores the print into a vector

    int size = code.size();
    for(int i = 0; i < size; i++){

        if(descriptor[i] == begin){ ///Checks the descriptor to match
            simWait(0);
            process++; /// if begin it will start a new process

            if(i == 0){
                PCB.setState(ready); /// A{begin} gets ready
                state.at(process - 1) = PCB;
            }
            else {
                PCB.setState(ready); /// A{begin} gets ready
                state.push_back(PCB); ///will create a vector to store the PCB object
            }

            toPrint.push_back(to_string(elapsed_time) + " - OS: preparing process " + to_string(process) + "\n"); ///stores the print into a vector
            elapsed_time += simWait(cycles[i]); /// will call the wait function and passed the ms needed to wait. returns elapsed time to update running total
            toPrint.push_back(to_string(elapsed_time) + " - OS: starting process " + to_string(process) + "\n"); ///stores the print into a vector
        }
        else if(descriptor[i] == run){ ///Checks the descriptor to match
            PCB.setState(running); /// will set the PCB state
            state.at(process - 1) = PCB; ///will update the PCB state of the process
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": starting processing action\n");///stores the print into a vector
            elapsed_time += simWait(cycles[i]);/// will call the wait function and passed the ms needed to wait. returns elapsed time to update running total
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": ending processing action\n");///stores the print into a vector
        }

        ///same concept for the rest of the if statements

        else if(descriptor[i] == allocate){ /// Memory does not get state
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": allocating memory\n");
            elapsed_time += simWait(cycles[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": memory allocated at " + generateHexString() + "\n");
        }
        else if(descriptor[i] == monitor){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start monitor output\n");
            elapsed_time += execute(cycles[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end monitor output\n");
        }
        else if(descriptor[i] == hard_drive){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start hard drive input\n");
            elapsed_time += execute(cycles[i]); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end hard drive input\n");
        }
        else if(descriptor[i] == scanner){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start scanner output\n");
            elapsed_time += execute(cycles[i]);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end scanner output\n");
        }
        else if(descriptor[i] == projector){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start projector output\n");
            elapsed_time += execute(cycles[i]);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end projector output\n");
        }
        else if(descriptor[i] == block){ /// Memory gets no state
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start memory blocking\n");
            elapsed_time += simWait(cycles[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end memory blocking\n");
        }
        else if(descriptor[i] == keyboard){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start keyboard input\n");
            elapsed_time += execute(cycles[i]); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end keyboard input\n");
        }
        else if(descriptor[i] == finish){
            PCB.setState(exitProcess);
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - OS: removing process " + to_string(process) + "\n");
        }
    }

    PCB.setState(exitProcess); ///finish gets exit, saved from meta
    state.at(process - 1) = PCB;
    toPrint.push_back(to_string(elapsed_time) + " - Simulator program ending\n");
}

/**
 * @brief Will generate a random number and convert that into a hexadecimal string
 * @return returns the random number in a hexadecimal string
 */
string simulation::generateHexString() {

    string hexStr;
    srand (time(NULL));
    unsigned int num = rand() % 65536 + 1; /// largest number in hexadecimal

    //cout << " INT " << num << endl;

    /// integer value to hex-string
    stringstream sstream;
    sstream << "0x"
            << std::setfill ('0') << std::setw(2)
            << std::hex << num;

    hexStr= sstream.str();
    sstream.clear();    ///clears out the stream-string

    //transform(hexStr.begin(), hexStr.end(),hexStr.begin(), ::toupper);
    return hexStr;
}

/**
 * @brief Prints out to the monitor
 */
void simulation::printMonitor() {
    for (unsigned int i = 0; i < toPrint.size() ; ++i)
        cout << toPrint[i];
}

/**
 * @brief Prints out to the file
 */
void simulation::printFile(config& x) {
    ofstream log;
    log.open(x.getPath(), std::ios_base::app);
    for (unsigned int i = 0; i < toPrint.size() ; ++i)
        log << toPrint[i];
    log.close();
}

/**
 * @brief Pthread function that creates a thread, will also call the wait function
 */

void *sim_thread(void *duration) {

    double dur = *(double*) duration;
    simulation newSim;
    newSim.simWait(dur);
    pthread_exit(NULL);
}

/**
 *@brief Helper function that creates the pthread and joins them together. Also takes the elapsed
 * time for the it to run
 */

double simulation::execute(double duration) {

    pthread_t newThread;
    int x;
    double* temp = new double;
    *temp = duration;
    auto start = chrono::high_resolution_clock::now();
    x = pthread_create(&newThread, NULL, sim_thread, (void*) temp); /// creates pthread

    if(x){
        exit(EXIT_FAILURE);
    }

    (void) pthread_join(newThread, NULL); /// joins the threads

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    return elapsed.count();
}

/**
 * @brief prints out the data copied from meta class, used for debugging
 */
void simulation::printVector() {

    for (unsigned int i = 0; i < code.size() ; ++i) {
        cout << "code: " << code[i] << " desc: " << descriptor[i] << endl;
     }
}


