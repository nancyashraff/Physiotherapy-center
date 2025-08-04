#include "ExerciseTreatment.h"
#include "Scheduler.h"
#include "Treatment.h"

bool ExerciseTreatment::canAssign(Scheduler& scheduler) const
{
    return scheduler.hasAvailableExerciseRoom();
}

void ExerciseTreatment::moveToWait(Scheduler& scheduler, Patient* patient)
{
    scheduler.addToXWait(patient);
}