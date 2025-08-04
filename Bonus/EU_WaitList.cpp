#include "EU_WaitList.h"
#include"Patient.h"

void EU_WaitList::insertSorted(Patient* patient)
{
    if (isEmpty())
    {
        enqueue(patient);
        return;
    }

    LinkedQueue<Patient*> tempQueue;
    bool inserted = false;
    int newPriority = patient->getEffectivePriority();

    Patient* currentPatient;
    while (dequeue(currentPatient))
    {
        if (!inserted && newPriority < currentPatient->getEffectivePriority())
        {
            tempQueue.enqueue(patient);
            inserted = true;
        }
        tempQueue.enqueue(currentPatient);
    }

    if (!inserted)
    {
        tempQueue.enqueue(patient);
    }

    while (tempQueue.dequeue(currentPatient))
    {
        enqueue(currentPatient);
    }
}

int EU_WaitList::calcTreatmentLatency() const
{
    int totalLatency = 0;
    LinkedQueue<Patient*> tempQueue(*this);

    Patient* currentPatient;
    while (tempQueue.dequeue(currentPatient))
    {
        totalLatency += currentPatient->getCurrentTreatmentDuration();
    }

    return totalLatency;
}