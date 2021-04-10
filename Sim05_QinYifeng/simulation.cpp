//
// Created by Yifeng Qin on 3/8/2019.
//
#include <iostream>
#include <chrono>
#include <fstream>
#include <string>
#include <algorithm>
#include <vector>
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
    sched = x.getScheduling();
    QuantNum = x.getQuantNum();
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
    for (unsigned int j = 0; j < process; ++j) {
        state.push_back(PCB);
    }
    toPrint.push_back(to_string(elapsed_time) + " - Simulator program starting" + "\n"); ///stores the print into a vector

    int size = code.size();
    for(int i = 0; i < size; i++){

        if(sorted[i] == begin){ ///Checks the descriptor to match
            simWait(0);

            if(i == 0){
                PCB.setState(ready); /// A{begin} gets ready
                state.at(sortedProcess[0]) = PCB;
            }
            else {
                PCB.setState(ready); /// A{begin} gets ready
                state.push_back(PCB); ///will create a vector to store the PCB object
            }

            toPrint.push_back(to_string(elapsed_time) + " - OS: preparing process " + to_string(sortedProcess[0] + 1) + "\n"); ///stores the print into a vector
            elapsed_time += simWait(sortedCycles[i]); /// will call the wait function and passed the ms needed to wait. returns elapsed time to update running total
            toPrint.push_back(to_string(elapsed_time) + " - OS: starting process " + to_string(sortedProcess[0] + 1) + "\n"); ///stores the print into a vector
        }
        else if(sorted[i] == run){ ///Checks the descriptor to match
            PCB.setState(running); /// will set the PCB state
            state.at(sortedProcess[0]) = PCB; ///will update the PCB state of the process
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": starting processing action\n");///stores the print into a vector
            elapsed_time += simWait(sortedCycles[i]);/// will call the wait function and passed the ms needed to wait. returns elapsed time to update running total
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": ending processing action\n");///stores the print into a vector
        }

        ///same concept for the rest of the if statements

        else if(sorted[i] == allocate){ /// Memory does not get state
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": allocating memory\n");
            elapsed_time += simWait(sortedCycles[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": memory allocated at " + setHexMemory() + "\n");
        }
        else if(sorted[i] == monitor){
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[0]) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": start monitor output\n");
            elapsed_time += execute(sortedCycles[i], monitorSemaphore, monitorLock);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": end monitor output\n");
        }
        else if(sorted[i] == hard_drive){
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[0]) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": start hard drive input on HDD " + to_string(idxHD) + "\n");
            setHDIndex();
            elapsed_time += execute(sortedCycles[i], hdSemaphore, hdLock); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": end hard drive input\n");
        }
        else if(sorted[i] == scanner){
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[0]) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": start scanner output\n");
            elapsed_time += execute(sortedCycles[i], scannerSemaphore, scannerLock);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": end scanner output\n");
        }
        else if(sorted[i] == projector){
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[0]) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": start projector output on PROJ " + to_string(idxProj) + "\n");
            setProjIndex();
            elapsed_time += execute(sortedCycles[i], projectorSemaphore, projectorLock);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": end projector output\n");
        }
        else if(sorted[i] == block){ /// Memory gets no state
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": start memory blocking\n");
            elapsed_time += simWait(sortedCycles[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": end memory blocking\n");
        }
        else if(sorted[i] == keyboard){
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[0]) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": start keyboard input\n");
            elapsed_time += execute(sortedCycles[i], keyboardSemaphore, keyboardLock); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads
            toPrint.push_back(to_string(elapsed_time) + " - Process: "  + to_string(sortedProcess[0] + 1) + ": end keyboard input\n");
        }
        else if(sorted[i] == finish){
            PCB.setState(exitProcess);
            state.at(sortedProcess[0]) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - OS: removing process " + to_string(sortedProcess[0] + 1) + "\n");
            sortedProcess.erase (sortedProcess.begin());
            currMem = 0; ///resets the memory after an application ends

        }

    }

    PCB.setState(exitProcess); ///finish gets exit, saved from meta
    state.at(sortedProcess[0]) = PCB;
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

/**
 * @brief first I create two vectors for the descriptors and cycle times. Then i create a
 * vector that stores the number or processes, I/O operations, and process number. I sort the
 * vector the number of processes and  I/O operations based on the sorting algorithm. Then I push
 * the process descriptors and cycles to a single vector in the correct order. Then those vectors are
 * runned by the simulation.
 */
void simulation::sortSched() {
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

    vector<vector<string>> vec; ///descriptor
    vector<vector<int>> vec2; /// cycles
    vector<vector<int>> data;
    int ps = 0; ///I/O
    int sjf = 0; ///tasks
    int index = 0;
    int size = code.size();

    for(int i = 0; i < size; i++) {
        vec.push_back(std::vector<string>());
        vec2.push_back(std::vector<int>());
        if(descriptor[i] == finish) {
            data.push_back(std::vector<int>());
            data[index].push_back(ps); /// third
            data[index].push_back(sjf); ///second
            data[index].push_back(index);///index is first position

            vec[index].push_back(descriptor[i]);
            vec2[index].push_back(cycles[i]);
            index++;
            sjf = 0; ///reset for each process
            ps = 0;
            process = index + 1;

        }
        else if(descriptor[i] == begin){
            vec[index].push_back(descriptor[i]);
            vec2[index].push_back(cycles[i]);
        }
        else if(descriptor[i] == hard_drive || descriptor[i] == projector || descriptor[i] == scanner || descriptor[i] == keyboard || descriptor[i] == monitor){
            vec[index].push_back(descriptor[i]);
            vec2[index].push_back(cycles[i]);
            ps++;
            sjf++;
        }
        else {
            vec[index].push_back(descriptor[i]);
            vec2[index].push_back(cycles[i]);
            sjf++;
        }

    }
    
    if(sched == "FIFO"){
        for(auto & i : data) { /// nothing needs to be sorted
            int idx = i[2];
            vector<string> copyRow(vec[idx].begin(), vec[idx].end()); ///makes a copy of a row in the 2d vector
            vector<int> copyCycles(vec2[idx].begin(), vec2[idx].end()); ///makes a copy of a row in the 2d vector
            appendVector(sorted, copyRow); ///appends it to a vector with the correct order based on sorting algorithm
            appendCycles(sortedCycles, copyCycles);///appends it to a vector with the correct order based on sorting algorithm
            sortedProcess.push_back(idx); /// saves the correct process
        }
    }
    else if(sched == "PS"){

        const int columnId = 0; ///Sorts the 2d vector by looking at first column containing the number of I/O operations
        sort(data.begin(), data.end(), [&columnId](const vector<int> &item1, const vector<int> &item2)///sorts the rows by column
        {
            return item1[columnId] < item2[columnId];
        });

        for(auto & i : data) {
            int idx = i[2];
            vector<string> copyRow(vec[idx].begin(), vec[idx].end());
            vector<int> copyCycles(vec2[idx].begin(), vec2[idx].end());
            appendVector(sorted, copyRow);
            appendCycles(sortedCycles, copyCycles);
            sortedProcess.push_back(idx);
        }
    }
    else if(sched == "SJF"){

        const int columnId = 1; ///Sorts the 2d vector by looking at first column containing the number of I/O operations
        sort(data.begin(), data.end(), [&columnId](const vector<int> &item1, const vector<int> &item2) ///sorts the rows by column
        {
            return item1[columnId] < item2[columnId];
        });

        for(auto & i : data) {
            int idx = i[2];
            vector<string> copyRow(vec[idx].begin(), vec[idx].end());
            vector<int> copyCycles(vec2[idx].begin(), vec2[idx].end());
            appendVector(sorted, copyRow);
            appendCycles(sortedCycles, copyCycles);
            sortedProcess.push_back(idx);
        }
    }

}

/**
 * @brief Takes a row from a  2d string vector and appends it to a another 1d vector.
 * @param v
 * @param w
 */
void simulation::appendVector(vector<string> &v, const vector<string> &w) {

    for(const auto & i : w)
        v.push_back(i);
}

/**
 * @brief Takes a row from a  2d int vector and appends it to a another 1d vector.
 * @param v
 * @param w
 */
void simulation::appendCycles(vector<int> &v, const vector<int> &w) {

    for(const auto & i : w)
        v.push_back(i);
}

/**
 * @brief For the RR this contains the algorithm to go between processes. It stores it in a 2d vector
 * and when time runs out in one it will pop it out. This will continue until there is not time
 * left inside the vector meaning has ended.
 */
void simulation::RR() {

    OrderProcess();

    int size = code.size();
    int tempCycles = 0;

    string begin ("begin");
    string finish("finish");

    vector<vector<string>> vecDes; ///descriptor
    vector<vector<int>> vecC; /// cycles
    vector<vector<int>> vecP; ///process

    int index = 0;

    for(int i = 0; i < size; i++) { /// moves it into a 2d vector, each row is a process
        if(descriptor[i] == begin) {
            vecDes.push_back(std::vector<string>());
            vecC.push_back(std::vector<int>());
            vecP.push_back(std::vector<int>());
            beginPrint[index] = true;
        }
        else if(descriptor[i] == finish) {
            index++;
        }
        else{
            vecP[index].push_back(index);
            vecDes[index].push_back(descriptor[i]);
            vecC[index].push_back(cycles[i]);
        }

    }

    process = vecC.size();
unsigned int colIdx = 0;

while(notZero(vecC)) {

    for (unsigned int rowIdx = 0; rowIdx < vecC.size(); ++rowIdx) { /// always looks in column one for each process

        tempCycles = vecC[rowIdx][colIdx] - QuantNum; ///subtracts the cycles from the quantum number

        if (vecC[rowIdx].size() <= colIdx) {

        }else if (tempCycles <= 0 && vecC[rowIdx][colIdx] >= 0) { ///checks if it is 0 or a negative number

            sorted.push_back(vecDes[rowIdx][colIdx] + "fin");
            sortedProcess.push_back(vecP[rowIdx][colIdx] + 1);
            RunTime.push_back(vecC[rowIdx][colIdx]);
            vecC[rowIdx].erase(vecC[rowIdx].begin()); /// pops it from vector
            vecDes[rowIdx].erase(vecDes[rowIdx].begin());
            vecP[rowIdx].erase(vecP[rowIdx].begin());

        } else {

            sorted.push_back(vecDes[rowIdx][colIdx]);
            sortedProcess.push_back(vecP[rowIdx][colIdx] + 1);
            RunTime.push_back(QuantNum);
            vecC[rowIdx][colIdx] = tempCycles;
        }

    }
}


}
/**
 *
 * @brief This goes through the 2d vector and adds all the values together
 * If it is not 0 it will return true to keep the function running.
 * @param vec
 * @return
 */
bool simulation::notZero(vector<vector<int>> vec) {

    int total = 0;

    for (unsigned int row = 0; row < vec.size(); ++row) {

        for (unsigned int col = 0; col < vec[row].size(); ++col) {

            total += vec[row][col];
        }
    }

    return 0 != total;
}

/**
 * @breif This adds a vector containing the process id for each task
 */
void simulation::OrderProcess() {

    int idx = 1;
    int size = cycles.size();

    for (int i = 0; i < size; ++i) {

        RRProcess.push_back(idx);

        if (descriptor[i] == "finish")
            idx++;
    }
}
/**
 * @brief Similar to the project 4 run. This runs specifically for the RR and STR
 * This goes through and gives a vector what to print to the terminal or file
 * @param PCB
 */

void simulation::runV5(PCB& PCB) {

    string begin("begin");
    string run("run");
    string allocate("allocate");
    string monitor("monitor");
    string hard_drive("harddrive");
    string scanner("scanner");
    string projector("projector");
    string finish("finish");
    string block("block");
    string keyboard("keyboard");
    string startStr("start");

    string runfin("runfin");
    string allocatefin("allocatefin");
    string monitorfin("monitorfin");
    string hard_drivefin("harddrivefin");
    string scannerfin("scannerfin");
    string projectorfin("projectorfin");
    string blockfin("blockfin");
    string keyboardfin("keyboardfin");
    string startStrfin("startfin");
    
    cout << "running... will print when finished\n|||||||||||||||||||||||||||||||||||" << endl;

    init(); /// intializes all the mutex and semaphores
    PCB.setState(start); /// S{begin} gets start because no errors in meta
    for (unsigned int j = 0; j < process; ++j) {
        state.push_back(PCB);
    }
    toPrint.push_back(
            to_string(elapsed_time) + " - Simulator program starting" + "\n"); ///stores the print into a vector

    int size = sorted.size();
    for (int i = 0; i < size; i++) {


        if (beginPrint[sortedProcess[i] - 1]) { ///Checks the descriptor to match
            simWait(0);


            if (i == 0) {
                PCB.setState(ready); /// A{begin} gets ready
                //state.at(sortedProcess[i] - 1) = PCB;
            } else {
                PCB.setState(ready); /// A{begin} gets ready
                state.push_back(PCB); ///will create a vector to store the PCB object
            }

            toPrint.push_back(to_string(elapsed_time) + " - OS: preparing process " + to_string(sortedProcess[i]) +
                              "\n"); ///stores the print into a vector
            elapsed_time += simWait(
                    RunTime[i]); /// will call the wait function and passed the ms needed to wait. returns elapsed time to update running total
            toPrint.push_back(to_string(elapsed_time) + " - OS: starting process " + to_string(sortedProcess[i]) +
                              "\n"); ///stores the print into a vector

            beginPrint[sortedProcess[i] - 1] = false;
        }

        if (sorted[i] == run) { ///Checks the descriptor to match
            PCB.setState(running); /// will set the PCB state
            state.at(sortedProcess[i] - 1) = PCB; ///will update the PCB state of the process
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) + ": starting processing action\n");///stores the print into a vector
            elapsed_time += simWait(RunTime[i]);/// will call the wait function and passed the ms needed to wait. returns elapsed time to update running total
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) + ": interrupt processing action\n");///stores the print into a vector
            elapsed_time += interupt();/// interrupt for 50 ms
        }

            ///same concept for the rest of the if statements

        else if (sorted[i] == allocate) { /// Memory does not get state
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": allocating memory\n");
            elapsed_time += simWait(RunTime[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": memory allocated at " + setHexMemory() + "\n");

        } else if (sorted[i] == monitor) {
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[i]- 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": start monitor output\n");
            elapsed_time += execute(RunTime[i], monitorSemaphore, monitorLock);


        } else if (sorted[i] == hard_drive) {
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[i]- 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": start hard drive input on HDD " + to_string(idxHD) + "\n");
            setHDIndex();
            elapsed_time += execute(RunTime[i], hdSemaphore,
                                    hdLock); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads

        } else if (sorted[i] == scanner) {
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[i]- 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": start scanner output\n");
            elapsed_time += execute(RunTime[i], scannerSemaphore,
                                    scannerLock);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads


        } else if (sorted[i] == projector) {
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[i]- 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": start projector output on PROJ " + to_string(idxProj) + "\n");
            setProjIndex();
            elapsed_time += execute(RunTime[i], projectorSemaphore,
                                    projectorLock);/// I/O functions need to use pthreads so instead it calls a different wait with pthreads


        } else if (sorted[i] == block) { /// Memory gets no state
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": start memory blocking\n");
            elapsed_time += simWait(RunTime[i]);

        } else if (sorted[i] == keyboard) {
            PCB.setState(wait); /// I/O functions get wait
            state.at(sortedProcess[i]- 1) = PCB;
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": start keyboard input\n");
            elapsed_time += execute(RunTime[i], keyboardSemaphore,
                                    keyboardLock); /// I/O functions need to use pthreads so instead it calls a different wait with pthreads

        }
        else if (sorted[i] == allocatefin) { /// Memory does not get state
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": allocating memory\n");
            elapsed_time += simWait(RunTime[i]);
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": memory allocated at " + setHexMemory() + "\n");
        } else if (sorted[i] == monitorfin) {
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": end monitor output\n");
        } else if (sorted[i] == hard_drivefin) {
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": end hard drive input\n");
        } else if (sorted[i] == scannerfin) {
           toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": end scanner output\n");
        } else if (sorted[i] == projectorfin) {
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": end projector output\n");
        } else if (sorted[i] == blockfin) { /// Memory gets no state //////////////////////check if right
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": end memory blocking\n");
        } else if (sorted[i] == keyboardfin) {
            toPrint.push_back(to_string(elapsed_time) + " - Process: " + to_string(sortedProcess[i]) +
                              ": end keyboard input\n");
        }

    }

    PCB.setState(exitProcess); ///finish gets exit, saved from meta
    state.at(sortedProcess[sorted.size()]) = PCB;
    toPrint.push_back(to_string(elapsed_time) + " - Simulator program ending\n");
}

/**
 * @brief For the STR this contains the algorithm to go between processes. It stores it in a 2d vector
 * and when time runs out in one it will pop it out. This will continue until there is not time
 * left inside the vector meaning has ended.
 */
void simulation::STR() {

    OrderProcess();

    int size = code.size();
    int tempCycles = 0;

    string begin ("begin");
    string finish("finish");

    vector<vector<string>> vecDes; ///descriptor
    vector<vector<int>> vecC; /// cycles
    vector<vector<int>> vecP; ///process

    int index = 0;

    for(int i = 0; i < size; i++) { /// moves it into a 2d vector, each row is a process

        if(descriptor[i] == begin) {
            vecDes.push_back(std::vector<string>());
            vecC.push_back(std::vector<int>());
            vecP.push_back(std::vector<int>());
            beginPrint[index] = true;
        }
        else if(descriptor[i] == finish) {
            index++;
        }
        else{
            vecP[index].push_back(index);
            vecDes[index].push_back(descriptor[i]);
            vecC[index].push_back(cycles[i]);
        }

    }


    process = vecC.size();
    unsigned int colIdx = 0;

    while(notZero(vecC)) {

        int rowIdx = STRLow(vecC); ///gets the lowest number id

            tempCycles = vecC[rowIdx][colIdx] - QuantNum;

            if (vecC[rowIdx].size() <= colIdx) {

            }else if (tempCycles <= 0 && vecC[rowIdx][colIdx] >= 0) {

                sorted.push_back(vecDes[rowIdx][colIdx] + "fin");
                sortedProcess.push_back(vecP[rowIdx][colIdx] + 1);
                RunTime.push_back(vecC[rowIdx][colIdx]);
                vecC[rowIdx].erase(vecC[rowIdx].begin());
                vecDes[rowIdx].erase(vecDes[rowIdx].begin());
                vecP[rowIdx].erase(vecP[rowIdx].begin());


            } else {
                sorted.push_back(vecDes[rowIdx][colIdx]);
                sortedProcess.push_back(vecP[rowIdx][colIdx] + 1);
                RunTime.push_back(QuantNum);
                vecC[rowIdx][colIdx] = tempCycles;
            }

    }

}
/**
 * @breif Finds the lowest value for STR and returns what index it is at
 * @param vec
 * @return
 */
int simulation::STRLow(vector<vector<int>> vec) {

    int rows = vec.size();
    int col = 0;
    int lowIdx = 0;

    for (int i = 0; i < rows; ++i) {


        if (vec[i].empty()) {

            while(vec[i].empty()){
                i++;
                lowIdx = i;
            }

        }

        if (vec[i][col] < vec[lowIdx][col]){
            lowIdx = i;
        }

    }

    return lowIdx;
}

/**
 *
 * @brief Interupt for 50 ms
 * @return
 */
double simulation::interupt() {
    return simWait(50);
}

