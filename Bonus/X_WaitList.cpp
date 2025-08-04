#include "X_WaitList.h"
#include <cstdlib> 
#include <ctime>   
#include "EU_WaitList.h"
#include"Patient.h"


Patient* X_WaitList::cancel()
{
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    if (rand() % 100 >= Pcancel || isEmpty())
    {
        return NULL;
    }
    Patient* pcancelled = nullptr;
    int count = getCount();
    int targetIndex = rand() % count;
    int currentIndex = 0;
    Patient* current;

    while (count--)
    {
        dequeue(current);
        if (currentIndex++ == targetIndex)
        {
            if(current->isLastTreatmentXTherapy())
                pcancelled = current;
            else
                enqueue(current);
        }
        else
        {
            enqueue(current);
        }
    }
    if(pcancelled)
        pcancelled->setCancelled();
    return pcancelled;

}

void X_WaitList::SetPcancel(int Canc)
{
    Pcancel = Canc;
}


Patient* X_WaitList::Tempcancel()
{
    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    int originalCount = getCount();
    Patient* current;

    int EligiblePatients_Count = 0;
    for (int i = 0; i < originalCount; ++i)
    {
        if (dequeue(current)) {
            if (current->isLastTreatmentXTherapy())
                EligiblePatients_Count++;
            enqueue(current);
        }
    }

    if (EligiblePatients_Count == 0)
        return nullptr;

    // Loop until we find and remove a random eligible patient
    while (true)
    {
        int targetIndex = rand() % EligiblePatients_Count;
        int currentIndex = 0;

        for (int i = 0; i < originalCount; ++i)
        {
            if (!dequeue(current)) break;

            if (current->isLastTreatmentXTherapy())
            {
                if (currentIndex == targetIndex)
                {
                    // Found the patient to cancel
                    return current;
                }
                currentIndex++;
            }

            enqueue(current);
        }
    }

    return nullptr;
}
