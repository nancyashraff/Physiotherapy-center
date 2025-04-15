#pragma once
#include"LinkedQueue.h"
class Patient;

class EU_WaitList : public LinkedQueue<Patient*>
{
public:
    void insertSorted(Patient* patient);

    int calcTreatmentLatency() const;
};
