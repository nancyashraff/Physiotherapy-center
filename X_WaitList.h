#pragma once
#include "EU_WaitList.h"

class X_WaitList : public EU_WaitList 
{
    int CancelProbNumber;
    int Pcancel; // Cancellation probability (0-100)
public:
    X_WaitList(int cancelProb) : Pcancel(cancelProb),CancelProbNumber(0) {}

    Patient* cancel();

    flaot getCancellationPercent();
};

