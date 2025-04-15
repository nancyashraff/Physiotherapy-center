#pragma once
#include "LinkedQueue.h"
#include "PriQueue.h"
#include "ArrayStack.h"
#include"EU_WaitList.h"
#include"X_WaitList.h"
#include"EarlyPList.h"
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
    // Core Patient Lists
    LinkedQueue<Patient*> ALLPatients;
    EarlyPList EarlyPatients;
    PriQueue<Patient*> LatePatients;

    // Waiting Lists for Treatments
    EU_WaitList U_Waiting;
    EU_WaitList E_Waiting;
    X_WaitList X_Waiting;

    LinkedQueue<Patient*> RandomWaiting;

    // Treatment Resources
    LinkedQueue<Resource*> E_Devices;
    LinkedQueue<Resource*> U_Devices;
    LinkedQueue<Resource*> X_Rooms;

    // Currently in treatment
    PriQueue<Patient*> InTreatment;

    // Finished
    ArrayStack<Patient*> FinishedPatients;

    // UI Object
    UI UIobj;

public:
    // Constructor
    Scheduler();

    // Input/Initialization
    void LoadPatientsFromFile(string fileName);
    void InitializeResources(int NE, int NU, int NG, const int* gymCapacities);

    // Simulation core
    void RunSimulation();
    void ProcessTimeStep(int currentTime);
    void MoveFromEarlyToRandomWaiting();
    void MoveFromLateToRandomWaiting();
    void MoveFromRandomWaitingToInTreatment(int count, int currentTime);
    //void MoveFromRandomWaitingToInTreatment(int count);
    void MoveFromInTreatmentToRandomWaiting();
    void MoveFromInTreatmentToFinish();
    void CancelRandomFromWaiting();
    void RescheduleFromEarly();
    void HandleArrivals(int currentTime);
    void AssignTreatments(int currentTime);
    void UpdateInTreatment(int currentTime);
    void MoveToFinished(Patient* p);

    int getLatency(Treatment* t)const;
    // Utilities
    void PrintStatistics();

    //treatments checks
    bool hasAvailableUltrasoundDevice();
    void addToUWait(Patient* patient);

    bool hasAvailableExerciseRoom();
    void addToXWait(Patient* patient);

    bool hasAvailableElectroDevice();
    void addToEWait(Patient* patient);
};