#pragma once
#include"EU_WaitList.h"
class Patient;
class EU_WaitList;

class X_WaitList : public EU_WaitList
{
    int Pcancel; // Cancellation probability (0-100)
public:
    X_WaitList(int cancelProb=0) :  Pcancel(cancelProb) {}

    Patient* cancel();
    void SetPcancel(int Canc);
    Patient* Tempcancel();
};
