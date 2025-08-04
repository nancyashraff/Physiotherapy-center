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
    LinkedQueue<Patient*> ALLPatients;   // Queue of all patients in the system
    EarlyPList EarlyPatients;           // List of patients who arrived early
    PriQueue<Patient*> LatePatients;    // Priority queue for late arriving patients
    EU_WaitList U_Waiting;              // Waiting list for ultrasound treatment
    EU_WaitList E_Waiting;              // Waiting list for electrotherapy treatment
    X_WaitList X_Waiting;               // Waiting list for exercise treatment
    LinkedQueue<Resource*> E_Devices;    // Available electrotherapy devices
    LinkedQueue<Resource*> U_Devices;    // Available ultrasound devices
    LinkedQueue<Resource*> X_Rooms;      // Available exercise rooms
    PriQueue<Patient*> InTreatment;     // Patients currently receiving treatment
    ArrayStack<Patient*> FinishedPatients; // Patients who completed all treatments
    UI UIobj;                           // User interface object

public:
    Scheduler();
    ~Scheduler();

    // Loads patient data from specified file
     
    void LoadPatientsFromFile(string fileName);

    // Initializes treatment resources
    void InitializeResources(int NE, int NU, int NG, const int* gymCapacities);

    // Main simulation that runs the scheduling system 
    void RunSimulation();

    // Processes one timestep of the simulation
    void ProcessSimulation(int currentTime);

    // Handles new patient arrivals at current time
    void HandleArrivals(int currentTime);

    // Assigns patients to available Exercise rooms
    void AssignX(int currentTime);

    // Assigns patients to available Ultrasound devices
    void AssignU(int currentTime);

    // Assigns patients to available electrotherapy devices
    void AssignE(int currentTime);

    // Updates status of patients currently in treatment
    void UpdateInTreatment(int currentTime);

    // Moves a patient to finished state after completing treatment
    void MoveToFinished(Patient* p);

    int getLatency(Treatment* t)const;    // Returns latency for a given treatment waiting list

    bool hasAvailableUltrasoundDevice();    // Checks if there is an available ultrasound device    
    void addToUWait(Patient* patient);    // Adds a patient to the ultrasound waiting list

    bool hasAvailableExerciseRoom();    // Checks if there is an available exercise room
    void addToXWait(Patient* patient);    // Adds a patient to the exercise waiting list

    bool hasAvailableElectroDevice();    // Checks if there is an available electrotherapy device
    void addToEWait(Patient* patient);    // Adds a patient to the electrotherapy waiting list

    void incrementWaitingTime();    // Increment waiting time for all waiting patients
    void updateEarly_Late_list(int currentTime);    // Update the list of early and late patients
    void Cancel_And_Reshcudle();    // Rescheduled and cancelled patients   

    // Outputs the final results to a file
    void OutputFileMode(string fileName);
};