#pragma once
#include "LinkedQueue.h"
#include "PriQueue.h"
#include "ArrayStack.h"
#include "EU_WaitList.h"
#include "X_WaitList.h"
#include "EarlyPList.h"
#include "Treatment.h"
#include "UI.h"

class EarlyPList;
class EU_WaitList;
class X_WaitList;
class Patient;
class Resource;
class Treatment;

using namespace std;

 class Scheduler {
private:
    LinkedQueue<Patient*> ALLPatients;  
    EarlyPList EarlyPatients;           
    PriQueue<Patient*> LatePatients;   
    EU_WaitList U_Waiting; 
    EU_WaitList E_Waiting;
    X_WaitList X_Waiting;
    LinkedQueue<Resource*> E_Devices;
    LinkedQueue<Resource*> U_Devices;
    LinkedQueue<Resource*> X_Rooms;
    PriQueue<Patient*> InTreatment;
    ArrayStack<Patient*> FinishedPatients;
    UI UIobj;

    // Bonus Members
    PriQueue<Resource*> Maintenance;
    int U_FreeFailure;
    int U_BusyFailure;
    int U_MaintenanceDuration;

    int E_BusyFailure;
    int E_FreeFailure;
    int E_MaintenanceDuration;
    LinkedQueue<Patient*> U_InterruptPatients;
    LinkedQueue<Patient*> E_InterruptPatients;

public:
    Scheduler();

    void LoadPatientsFromFile(string fileName);
    void InitializeResources(int NE, int NU, int NG, const int* gymCapacities);

    // Simulation core
    void RunSimulation();
    void HandleArrivals(int currentTime);
    void AssignX(int currentTime);
    void AssignU(int currentTime);
    void AssignE(int currentTime);
    void UpdateInTreatment(int currentTime);
    void MoveToFinished(Patient* p);

    int getLatency(Treatment* t)const;

    bool hasAvailableUltrasoundDevice();
    void addToUWait(Patient* patient);

    bool hasAvailableExerciseRoom();
    void addToXWait(Patient* patient);

    bool hasAvailableElectroDevice();
    void addToEWait(Patient* patient);

    void incrementWaitingTime();
    void updateEarly_Late_list(int currentTime);
    void Cancel_And_Reshcudle();

    void OutputFileMode(string fileName);

    // Bonus Methods    
    void UpdateMaintenance(int currentTime);
    void UpdateInterruptPatient(int currentTime);
};