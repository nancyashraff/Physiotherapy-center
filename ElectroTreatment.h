#pragma once
#include"Treatment.h"

class Treatment;
class Scheduler;

class ElectroTreatment : public Treatment {
public:
    ElectroTreatment(/*TreatmentType t,*/ int d) : Treatment(d) {}

    bool canAssign(Scheduler& scheduler) const;
    void moveToWait(Scheduler& scheduler, Patient* patient);
};