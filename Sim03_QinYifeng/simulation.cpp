//
// Created by Yifeng Qin on 3/8/2019.
//
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>
#include <sstream>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>
#include <pthread.h>
#include <semaphore.h>
#include "simulation.h"
using namespace std;

sem_t monitorSemaphore;
sem_t projectorSemaphore;
sem_t hdSemaphore;
sem_t scannerSemaphore;
sem_t keyboardSemaphore;
pthread_mutex_t monitorLock;
pthread_mutex_t projectorLock;
pthread_mutex_t hdLock;
pthread_mutex_t scannerLock;
pthread_mutex_t keyboardLock;

/**
 * @breif Default constructor
 */
simulation::simulation() {

    process = 0;
    elapsed_time = 0;
    numHD = 0;
    numProj = 0;
    blockSize = 0;
    memorySize = 0;
    idxHD = 0;
    idxProj = 0;
    currMem = 0;
}

/**
 * @brief Parameterized Constructor
 */

simulation::simulation(config& x) {

    process = 0;
    elapsed_time = 0;
    idxHD = 0;
    idxProj = 0;
    currMem = 0;
    numHD = x.getNumHD();
    numProj = x.getNumeProj();
    blockSize = x.getBlock();
    memorySize = x.getSysMem();
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

    cout << "running... will print when finished\n|||||||||||||||||||||||||||||||||||"  << endl;
    
    init(); /// intializes all the mutex and semaphores
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
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": memory allocated at " + setHexMemory() + "\n");
        }
        else if(descriptor[i] == monitor){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start monitor output\n");
            elapsed_time += execute(cycles[i], monitorSemaphore, monitorLock);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end monitor output\n");
        }
        else if(descriptor[i] == hard_drive){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start hard drive input on HDD " + to_string(idxHD) + "\n");
            setHDIndex();
            elapsed_time += execute(cycles[i], hdSemaphore, hdLock); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end hard drive input\n");
        }
        else if(descriptor[i] == scanner){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start scanner output\n");
            elapsed_time += execute(cycles[i], scannerSemaphore, scannerLock);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": end scanner output\n");
        }
        else if(descriptor[i] == projector){
            PCB.setState(wait); /// I/O functions get wait
            state.at(process - 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(process) + ": start projector output on PROJ " + to_string(idxProj) + "\n");
            setProjIndex();
            elapsed_time += execute(cycles[i], projectorSemaphore, projectorLock);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads
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
            elapsed_time += execute(cycles[i], keyboardSemaphore, keyboardLock); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads
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

    destructor(); ///destroy all the mutexes and semaphores
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
 * @brief Pthread function that creates a thread, will also call the wait function. Start of the semaphore lock
 */

void *sim_thread(void *duration) {

    double dur = *(double*) duration;
    simulation newSim;
    newSim.simWait(dur);

    pthread_exit(NULL);
}

/**
 *@brief Helper function that creates the pthread and joins them together. Also takes the elapsed
 * time for the it to run. Take in the semaphore and mutex for that I/O operation and uses that
 * to lock it.
 */

double simulation::execute(double duration, sem_t &semaphore, pthread_mutex_t &mutex) {

    pthread_t newThread;
    int x;
    double* temp = new double;
    *temp = duration;

    sem_wait(&semaphore); /// signal resource is being used
    pthread_mutex_lock(&mutex); ///locks it for critical section

    auto start = chrono::high_resolution_clock::now();
    x = pthread_create(&newThread, NULL, sim_thread, (void*) temp); /// creates pthread

    if(x){
        exit(EXIT_FAILURE);
    }

    (void) pthread_join(newThread, NULL); /// joins the threads

    auto end = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed = end - start;

    pthread_mutex_unlock(&mutex);/// end of critical section | unlock
    sem_post(&semaphore); ///signals resource is released

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
/**
 * @brief Increments the index for the projector index it is on. If that index is greater then the quanity
 * if will reset back to 0
 */
void simulation::setProjIndex() {

    idxProj++;
    if(idxProj > numProj - 1)
        idxProj = 0;
}

/**
 * @brief Increments the index for the hard drive index it is on. If that index is greater then the quanity
 * if will reset back to 0
 */
void simulation::setHDIndex() {

    idxHD++;
    if(idxHD > numHD - 1)
        idxHD = 0;
}

/**
 * @brief This function tracks the running total for the current memory location. It increments by the
 * memory block size and when the location is greater then the system memory it resets to 0. It turns the
 * location into a hex number and I fill it with '0x' and '0' to match 32bit size. It is converted into a string
 * and returned.
 * @return string that contains the hex address of memory location
 */
string simulation::setHexMemory() {


    string hexStr;
    /// integer value to hex-string

    stringstream sstream;
    sstream << "0x"<< setfill ('0') << setw(8) << std::hex << currMem;

    hexStr = sstream.str();
    sstream.clear();

    currMem += blockSize; ///moves memory location a memory block

    if(currMem > memorySize) ///if memory reaches size, reset
        currMem = 0;

    return hexStr;

}

/**
 * @brief initialization for mutexes and semaphores
 */
void simulation::init() {

    /// set quantity of resources
    sem_init(&hdSemaphore, 0, numHD);
    sem_init(&monitorSemaphore, 0, 1); ///isn't given so i default to 1
    sem_init(&keyboardSemaphore, 0, 1);
    sem_init(&scannerSemaphore, 0, 1);
    sem_init(&projectorSemaphore, 0, numProj);

    pthread_mutex_init(&projectorLock, NULL);
    pthread_mutex_init(&hdLock, NULL);
    pthread_mutex_init(&monitorLock, NULL);
    pthread_mutex_init(&scannerLock, NULL);
    pthread_mutex_init(&keyboardLock, NULL);
}

/**
 * @brief destructor for mutexs and semaphores created
 */
void simulation::destructor() {

    sem_destroy(&hdSemaphore);
    sem_destroy(&monitorSemaphore);
    sem_destroy(&keyboardSemaphore);
    sem_destroy(&scannerSemaphore);
    sem_destroy(&projectorSemaphore);

    pthread_mutex_destroy(&projectorLock);
    pthread_mutex_destroy(&hdLock);
    pthread_mutex_destroy(&monitorLock);
    pthread_mutex_destroy(&scannerLock);
    pthread_mutex_destroy(&keyboardLock);
}


