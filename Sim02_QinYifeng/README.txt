Yifeng Qin
CS446
Project 2
3/15/19

File Names:
main.cpp config.h config.cpp meta.h meta.cpp PCB.h PCB.cpp simulation.h simulation.cpp makefile
Also include the files I tested with, configuation.conf and Test_1a.mdf

Compile:
There is a makefile, the executable should be ./sim2 ____________ with the configuration file name.

Design: 
To carry on from project one, I added two new classes. One is PCB and that contains the processState along with get and set functions for it. The simulation is ran in another class called simulation. I store the meta code, descriptor, and time in 3 separate vectors. I check the descriptor to run the appropriate wait, state update, and save what to print. There is a string vector that stores what to print. The simWait uses the systemclock to pause for the time it was give. The I/O was different in that it needed threads so it called to make a thread and simWait is used in the creation of the thread. 

To handle the PCB classes I have a vector of PCB objects that stores the state for each process. I create a new PCB object if there is a A{begin}. I update the processState with a set function throught the simulation. All the state changes are set in the simulation.cpp. 

Notes:
For print, it doesn't print the last project out unless there is an error. So it doesn't print the config and metadata. Printing the simulation is seperate from the simulation itslef, so if everything is correct, it waits until the simulation is done to print. So you have to wait until it the simulation finishes and it will print all at once.

Location of Threads:
In simulation.cpp -
void *sim_thread(void *duration) - line 247
double simulation::execute(double duration) - line 260

called on lines - 151 - 158 - 165 - 172 - 184 - 267

Location of PCB:
In PCB.cpp
void PCB::setState(int x) - line 12

called on lines - 108 - 120 - 124 - 133 - 148 - 154 - 162 - 169 - 181 - 188 - 194
