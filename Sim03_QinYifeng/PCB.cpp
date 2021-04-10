//
// Created by Yifeng Qin on 3/8/2019.
//

#include "PCB.h"

PCB::PCB() {

    processState = 0;
}

void PCB::setState(int x) {

    processState = x;
}

int PCB::getState() {
    return processState;
}
