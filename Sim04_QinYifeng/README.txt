Yifeng Qin
CS446
Project 4
4/24/19

File Names:
main.cpp config.h config.cpp meta.h meta.cpp PCB.h PCB.cpp simulation.h simulation.cpp makefile
Also include the files I tested with, configuation.conf and Test.mdf
There is Cmake file, but ignore them. They are just for me.

Compile:
There is a makefile, the executable should be ./sim4 ____________ with the configuration file name.

Design:
This project builds off a lot from the last project. The memory allocation is the same but is reset back to 0 for each process. If the memory is bigger then the system memory it also resets to 0. When it ends the memory is freed. 
For the scheduling algorithm, I first take in which algorithm is specified. Then I read the meta file from start to finish saving all the descriptors and cycles in their own vector. Then I take those vectors and move each process into its own row in a 2d vector. I count the tasks and I/O operations in each process and store it in a corresponding vector. I sort the corresponding vector based on the sorting algorithm. Then I will push all the rows in the 2d vectors back to a 1d vector with the right order.


Notes:
For print, it doesn't print the last project out unless there is an error. So it doesn't print the config and metadata. Printing the simulation is seperate from the simulation itslef, so if everything is correct, it waits until the simulation is done to print. So you have to wait until the simulation finishes and it will print all at once.



