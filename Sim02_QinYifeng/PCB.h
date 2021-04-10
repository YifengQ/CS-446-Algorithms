//
// Created by Yifeng Qin on 3/8/2019.
//

#ifndef SIM02_QINYIFENG_PCB_H
#define SIM02_QINYIFENG_PCB_H


class PCB {

public:
    PCB();

    void setState(int);
    int getState();
private:

    int processState;
};


#endif //SIM02_QINYIFENG_PCB_H
