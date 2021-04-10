Yifeng Qin
CS446
Project 3
4/7/19

File Names:
main.cpp config.h config.cpp meta.h meta.cpp PCB.h PCB.cpp simulation.h simulation.cpp makefile
Also include the files I tested with, configuation.conf and Test.mdf
There is Cmake file, but ignore them. They are just for me.

Compile:
There is a makefile, the executable should be ./sim3 ____________ with the configuration file name.

Design: 
To add on to the last project we had to use mutex locks and semaphores. The sempahore is for the resources and mutex lock for the I/O. I had a seperate function for I/O thread creation so I added the mutex and semaphore in there. I created a mutex and semaphore for each I/O operation. When i find that I/O operation i pass in the mutex and semaphore for that I/O operation to a function to implement it. I also make sure to intialize the mutex and semaphores. The semaphores get the numeber/quanity intialized. Then I make sure to destory them at the end.

Notes:
For print, it doesn't print the last project out unless there is an error. So it doesn't print the config and metadata. Printing the simulation is seperate from the simulation itslef, so if everything is correct, it waits until the simulation is done to print. So you have to wait until the simulation finishes and it will print all at once.

Location of Mutexes and Semaphores
All in simulation.cpp file

Creation of Semaphores and Mutexes on lines 18-27

Intialized in init function on line 394 - called on line 143
Destructed in destructor function on line 413 - called on line 237

Semaphore and Mutex used in execute function on line 304

execute is called and specific mutex and semaphore is passed as parameters on lines 187 - 195 - 202 - 210



