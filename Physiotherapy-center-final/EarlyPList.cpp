#include "EarlyPList.h"
#include "Patient.h"
#include <cstdlib>
#include <ctime>


Patient* EarlyPList::reschedule()
{
    static bool seeded = false;
    if (!seeded) 
    {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    if (rand() % 100 >= Presc || isEmpty())
    {
        return nullptr;
    }

    if (getCount() == 1)
    {
        Patient* p;
        int pri;
        peek(p, pri);
        if (p->getRescheduleCount() == 3)
        {
            return nullptr;
        }
    }
    int count = getCount();
    int maxtry = 0;
    Patient* p;
    int pri;
    Patient* rescheduledPatient = nullptr;
    do{
        if(rescheduledPatient)
            enqueue(rescheduledPatient, pri);
        count = getCount();
        int targetIndex = rand() % count;
        int currentIndex = 0;
        while (count--)
        {
            dequeue(p, pri);
            if (currentIndex++ == targetIndex)
            {
                rescheduledPatient = p;
            }
            else
            {
                enqueue(p, pri);
            }
        }
        if (rescheduledPatient->getRescheduleCount() == 3)
        {
            enqueue(rescheduledPatient, pri);
            rescheduledPatient = nullptr;
        }
    }while(!rescheduledPatient && maxtry++ < count/2);

    
    if (rescheduledPatient)
    {
        //rescheduledPatient->setRescheduled();
        rescheduledPatient->incrementRescheduleCount();
        int delta = rand() % 10 + 1;
        rescheduledPatient->setPT(rescheduledPatient->getPT() + delta );
    }
    return rescheduledPatient;
}

void EarlyPList::SetPresc(int Resc)
{
    Presc = Resc;
}

Patient* EarlyPList::Tempreschedule()
{
    int count = getCount();
    if (count == 0) return nullptr;

    static bool seeded = false;
    if (!seeded) {
        srand(static_cast<unsigned>(time(nullptr)));
        seeded = true;
    }

    int targetIndex = rand() % count;
    int currentIndex = 0;

    Patient* rescheduledPatient = nullptr;
    Patient* p = nullptr;
    int pri;

    while (count > 0 && dequeue(p, pri))
    {
        if (currentIndex == targetIndex)
        {
            rescheduledPatient = p;
        }
        else
        {
            enqueue(p, pri);
        }

        currentIndex++;
        count--;
    }

    if (rescheduledPatient)
    {
        int delta = rand() % 5 + 1; // reschedule by 1�5 timesteps
        rescheduledPatient->setPT(rescheduledPatient->getPT() + delta);
        return rescheduledPatient;
    }

    return nullptr;
}

void EarlyPList::enqueue(Patient*& data, int priority)
{
    PriQueue::enqueue(data, priority);
}
