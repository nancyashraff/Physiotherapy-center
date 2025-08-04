#pragma once

#include "PriQueue.h"

class Patient;

class EarlyPList : public PriQueue<Patient*>
{
    int Presc; // Rescheduling probability (0-100)
public:
    EarlyPList(int rescheduleProb = 0) : Presc(rescheduleProb) {}

    Patient* reschedule();
    void SetPresc(int Resc);

    void enqueue(Patient*& data, int priority);
    Patient* Tempreschedule();
};


