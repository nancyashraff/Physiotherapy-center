#include "Patient.h"
#include "Scheduler.h"
#include "Treatment.h"
#include "ElectroTreatment.h"
#include "ExerciseTreatment.h"
#include "UltrasoundTreatment.h"
#include "Resource.h"
#include <fstream>
#include "cmath"

#include <climits>
int Patient::N_Patients = 0;
int Patient::R_Patients = 0;

Patient::Patient(int id, PatientType patientType, int appointmentTime, int arrivalTime, LinkedQueue<Treatment*>& treatments)
    : PID(id), type(patientType), PT(appointmentTime), VT(arrivalTime),
    status(PatientStatus::IDLE), currentTreatmentDuration(0),
    treatmentStartTime(-1)
{

    Treatment* treatment;
    while (treatments.dequeue(treatment))
    {
        requiredTreatments.enqueue(treatment);
    }
    updateStatus(0);
    if(patientType==PatientType::NORMAL)
        N_Patients++;
    else
        R_Patients++;
    Cancelled=false;
    Rescheduled=false;
    WT=0;
    TT=0;
    LastTreatmentStartTime=0;
    latePenalty=0;
    RescheduleCount=0;
}

void Patient::setState(PatientStatus p)
{
    status = p;
}

void Patient::setPT(int newPT)
{
    PT = newPT;
}

void Patient::setTreatmentStartTime(int t)
{
    treatmentStartTime = t;
}

void Patient::optimizeTreatmentOrder(Scheduler& scheduler)
{
    if (type != PatientType::RECOVERING || requiredTreatments.getCount() <= 1)
    {
        return;
    }

    LinkedQueue<Treatment*> tempQueue;
    Treatment* optimalTreatment = nullptr;
    int minLatency = INT_MAX;

    Treatment* t;
    while (requiredTreatments.dequeue(t))
    {
        tempQueue.enqueue(t);

        int currentLatency = scheduler.getLatency(t);
        if (currentLatency < minLatency)
        {
            minLatency = currentLatency;
            optimalTreatment = t;
        }
    }

    if (optimalTreatment)
    {
        requiredTreatments.enqueue(optimalTreatment);

        while (tempQueue.dequeue(t))
        {
            if (t != optimalTreatment)
            {
                requiredTreatments.enqueue(t);
            }
        }
    }
}

Treatment* Patient::getCurrentTreatment() const
{
    Treatment* t;
    if (requiredTreatments.peek(t))
    {
        return t;
    }
    return nullptr;
}

TreatmentType Patient::getCurrentTreatmentType() const
{
    Treatment* t = getCurrentTreatment();

        if (dynamic_cast<ElectroTreatment*>(t))
            return TreatmentType::ELECTRO;
        else if (dynamic_cast<ExerciseTreatment*>(t))
            return TreatmentType::EXERCISE;
        else
            return TreatmentType::ULTRASOUND;
}

bool Patient::hasMoreTreatments() const
{
    return !requiredTreatments.isEmpty();
}

bool Patient::isLastTreatmentXTherapy() const
{
    if (requiredTreatments.isEmpty()) 
        return false;
    if(requiredTreatments.getCount()!=1)
        return false;
    Treatment* t;
    requiredTreatments.peek(t);
    return dynamic_cast<ExerciseTreatment*>(t);
}

void Patient::startTreatment(int currentTime)
{
    treatmentStartTime = currentTime;
    Treatment* t = getCurrentTreatment();
    if (t) 
        currentTreatmentDuration = t->getDuration();
    status = PatientStatus::SERV;
}

bool Patient::completeTreatment()
{
    Treatment* t;
    if (requiredTreatments.dequeue(t))
    {
        delete t;
        currentTreatmentDuration = 0;
        if(!LastTreatmentStartTime)
            LastTreatmentStartTime=treatmentStartTime;
        treatmentStartTime = -1;
        return true;
    }
    return false;
}

void Patient::updateStatus(int currentTime)
{
    if (status == PatientStatus::FNSH) return;
    if (currentTime == -1)
        status = PatientStatus::FNSH;
    else if (VT > currentTime)
    {
        status = PatientStatus::IDLE;
    }
    else if (VT < PT)
    {
        status = PatientStatus::ERLV;
    }
    else if (VT > PT)
    {
        status = PatientStatus::LATE;
        calculateLatePenalty();
    }
    else
    {
        status = PatientStatus::WAIT;
    }
}

void Patient::calculateLatePenalty()
{
    if (VT > PT)
    {
        latePenalty = ceil((VT - PT) / 2.0);
    }
}

void Patient::print(ostream& os) const
{
    if (status == PatientStatus::ERLV || status == PatientStatus::LATE ||
        status == PatientStatus::WAIT || status == PatientStatus::FNSH)
    {
        os << getPID() << " ";
    }
    else if (status == PatientStatus::IDLE)
    {
        os << "P" << getPID() << "_" << getVT() << " ";
    }
    else if(status==PatientStatus::SERV)
    {
        os << "P" << getPID() << "_";
        if (getCurrentTreatmentType() == TreatmentType::ELECTRO)
        {
            os << "E" << ResourceID()<<", ";
        }
        else if (getCurrentTreatmentType() == TreatmentType::EXERCISE)
        {
            os << "R" << ResourceID() << ", ";
        }
        else
        {
            os << "U" << ResourceID() << ", ";
        }
    }
}

ostream& operator<<(ostream& os, const Patient* patient)
{
    patient->print(os);
    return os;
}

ofstream& operator<<(ofstream& Of, const Patient* patient)
{
    Of << "P" << patient->getPID() << "\t";
    Of << (patient->getType() == PatientType::NORMAL ? "N" : "R") << "\t";
    Of << patient->getPT() << "\t";
    Of << patient->getVT() << "\t";
    Of << ((!patient->getLatePenalty())? patient->getPT() + patient->getWT() + patient->getTT() 
        : patient->getVT() + patient->getWT() + patient->getTT() + patient->getLatePenalty()) << "\t";
    Of << patient->getWT() << "\t";
    Of << patient->getTT() << "\t";
    Of << (patient->getCancelled()?"T":"N") << "\t";
    Of << (patient->getRescheduled()?"T":"N") << "\n";
    return Of;
}


int Patient::ResourceID() const
{
    Treatment* treat = nullptr;
    requiredTreatments.peek(treat);
    Resource* res = treat->getAssignedResource();
    return res->get_id();

}
void Patient:: updateTT(int duration)
{
    TT+=duration;
}

void Patient::incrementWT()
{
    WT++;
}

void Patient::setCancelled()
{
    Cancelled=true;
}
void Patient::setRescheduled()
{
    Rescheduled=true;
}

Patient::~Patient() 
{
    Treatment* treatment;
    while (requiredTreatments.dequeue(treatment)) 
    {
        delete treatment;
    }
}
int Patient::getLastTreatmentStartTime() const
{
    return LastTreatmentStartTime;
}

int Patient::getCurrentTreatmentDuration() 
{
    Treatment* t = getCurrentTreatment();
    if (t)
        currentTreatmentDuration = t->getDuration();
    return currentTreatmentDuration;
}

void Patient::setUinterrupt()
{
    Uinterrupt=true;
}

void Patient::setEinterrupt()
{
    Einterrupt=true;
}

void Patient::incrementRescheduleCount()
{
    RescheduleCount++;
}