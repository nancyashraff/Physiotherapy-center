#ifndef EXERCISE_TREATMENT_H
#define EXERCISE_TREATMENT_H

#include"Treatment.h"

class Scheduler;
class Treatment;

class ExerciseTreatment : public Treatment {
public:
    ExerciseTreatment(/*TreatmentType t,*/ int d) : Treatment(d) {}

    bool canAssign(Scheduler& scheduler) const;
    void moveToWait(Scheduler& scheduler, Patient* patient);
};

#endif // EXERCISE_TREATMENT_H