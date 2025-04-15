#include "UltrasoundTreatment.h"
#include "Scheduler.h"
#include "Treatment.h"

bool UltrasoundTreatment::canAssign(Scheduler& scheduler) const
{
    return scheduler.hasAvailableUltrasoundDevice();
}

void UltrasoundTreatment::moveToWait(Scheduler& scheduler, Patient* patient)
{
    scheduler.addToUWait(patient);
}