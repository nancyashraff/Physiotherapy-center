#include "Scheduler.h"
#include "EU_WaitList.h"
#include "EarlyPList.h"
#include "ElectroTreatment.h"
#include "ExerciseTreatment.h"
#include "Patient.h"
#include "Resource.h"
#include "Treatment.h"
#include "UI.h"
#include "UltrasoundTreatment.h"
#include "X_WaitList.h"
#include "iostream"
#include <cctype>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <sstream>

using namespace std;

Scheduler::Scheduler() {
  srand(time(nullptr));
}

Scheduler::~Scheduler()
{
    Patient* delP = NULL;
    Resource* delRes = NULL;
    while(FinishedPatients.pop(delP))
        delete delP;
    while(E_Devices.dequeue(delRes))
        delete delRes;
    while(U_Devices.dequeue(delRes))
        delete delRes;
    while(X_Rooms.dequeue(delRes))
        delete delRes;
}

void Scheduler::LoadPatientsFromFile(string fileName) {
  ifstream file(fileName);
  if (!file.is_open()) {
    cout << "can not open file\n";
    return;
  }
  int NE, NU, NG;
  file >> NE >> NU >> NG;

  int *gymCapacities = new int[NG];
  for (int i = 0; i < NG; ++i) {
    file >> gymCapacities[i];
  }

  InitializeResources(NE, NU, NG, gymCapacities);

  int Pcancel, Presc;
  file >> Pcancel >> Presc;
  X_Waiting.SetPcancel(Pcancel);
  EarlyPatients.SetPresc(Presc);

  int Patients;
  file >> Patients;

  for (int i = 0; i < Patients; ++i) {
    char typeChar;
    int PT, VT, NT;
    file >> typeChar >> PT >> VT >> NT;

    if (NT > 3) {
      for (int j = 0; j < NT; ++j) {
        char dummyType;
        int dummyDur;
        file >> dummyType >> dummyDur;
      }
      continue;
    }

    PatientType type =
        (typeChar == 'N') ? PatientType::NORMAL : PatientType::RECOVERING;

    LinkedQueue<Treatment *> treatments;

    bool hasElectro = false, hasUltrasound = false, hasExercise = false;
    bool valid = true;

    for (int j = 0; j < NT; ++j) {
      char treatTypeChar;
      int duration;
      file >> treatTypeChar >> duration;
      treatTypeChar = toupper(treatTypeChar);

      if (treatTypeChar == 'E' && hasElectro) {
        valid = false;
        break;
      }
      if (treatTypeChar == 'U' && hasUltrasound) {

        valid = false;
        break;
      }
      if (treatTypeChar == 'X' && hasExercise) {

        valid = false;
        break;
      }

      // Create appropriate treatment
      Treatment *newTreatment = nullptr;
      switch (treatTypeChar) {
      case 'E':
        newTreatment = new ElectroTreatment(duration);
        hasElectro = true;
        break;
      case 'U':
        newTreatment = new UltrasoundTreatment(duration);
        hasUltrasound = true;
        break;
      case 'X':
        newTreatment = new ExerciseTreatment(duration);
        hasExercise = true;
        break;
      default:
        valid = false;
      }

      if (!valid)
        break;
      treatments.enqueue(newTreatment);
    }

    if (valid) {
      Patient *newPatient = new Patient(i + 1, type, PT, VT, treatments);
      ALLPatients.enqueue(newPatient);
    } else {
      // Clean up any treatments we created
      Treatment *t;
      while (treatments.dequeue(t)) {
        delete t;
      }
      // Skip remaining treatments if we aborted early
      for (int j = treatments.getCount(); j < NT; ++j) {
        char dummyType;
        int dummyDur;
        file >> dummyType >> dummyDur;
      }
    }
  }
}

void Scheduler::InitializeResources(int NE, int NU, int NG,
                                    const int *gymCapacities) {
  for (int i = 0; i < NE; ++i)
    E_Devices.enqueue(new Resource(TreatmentType::ELECTRO));

  for (int i = 0; i < NU; ++i)
    U_Devices.enqueue(new Resource(TreatmentType::ULTRASOUND));

  for (int i = 0; i < NG; ++i)
    X_Rooms.enqueue(new Resource(TreatmentType::EXERCISE, gymCapacities[i]));
  delete [] gymCapacities;
}

void Scheduler::RunSimulation()
{
  int mode = UIobj.ReadInterfaceMode();
    if(mode>2 || mode<1)
    {
        cout<<"Invalid Mode\n";
        return;
    }
  int currentTime = 0;
  cout << "Enter input file name: ";
  string FileName = UIobj.getFilename();
  LoadPatientsFromFile(FileName);
  if (mode == 1)
  while (!ALLPatients.isEmpty() || !InTreatment.isEmpty() ||
         !EarlyPatients.isEmpty() || !LatePatients.isEmpty() ||
         !E_Waiting.isEmpty() || !U_Waiting.isEmpty() || !X_Waiting.isEmpty()) {
      ProcessSimulation(currentTime);
      UIobj.printConsole(currentTime, &ALLPatients, &EarlyPatients, &LatePatients,
       &E_Waiting, &U_Waiting, &X_Waiting, &InTreatment,
       &FinishedPatients, &E_Devices, &U_Devices, &X_Rooms);
      currentTime++;
    cout << "\nPress any key to display next timestep..." << endl;
    if (cin.get())
    continue;
    }
    else
    {
      cout<<"Silent Mode, Simulation ends, Output file to be created\n";
      while (!ALLPatients.isEmpty() || !InTreatment.isEmpty() ||
             !EarlyPatients.isEmpty() || !LatePatients.isEmpty() ||
             !E_Waiting.isEmpty() || !U_Waiting.isEmpty() || !X_Waiting.isEmpty()) {
        ProcessSimulation(currentTime);
        currentTime++;
      }
    }
      cout << "Enter output file name: ";
      FileName = UIobj.getFilename();
      OutputFileMode(FileName);
      cout << "Output file created successfully.\n";  
}

void Scheduler::ProcessSimulation(int currentTime) 
{
  HandleArrivals(currentTime);
  updateEarly_Late_list(currentTime);
  UpdateInTreatment(currentTime);
  AssignX(currentTime);
  AssignU(currentTime);
  AssignE(currentTime);
  incrementWaitingTime();
  Cancel_And_Reshcudle();
}

void Scheduler::HandleArrivals(int currentTime) {
  if (!currentTime)
    return;
  int size = ALLPatients.getCount();
  for (int i = 0; i < size; ++i) {
    Patient *p;
    ALLPatients.dequeue(p);
    if (p->getVT() > currentTime) 
    {
      ALLPatients.enqueue(p);
      p->setState(PatientStatus::IDLE);
    } 
    else if (p->getVT() < p->getPT()) 
    {
      EarlyPatients.enqueue(p, -(p->getPT()));
      p->setState(PatientStatus::ERLV);
    }
    else if (p->getVT() > p->getPT()) 
    {
      p->calculateLatePenalty();
      int prio = -(p->getLatePenalty() + p->getVT()); // get priority
      LatePatients.enqueue(p, prio);
      p->setState(PatientStatus::LATE);
    } 
    else 
    {
      if (p->getType() == PatientType::RECOVERING)
      {
          p->optimizeTreatmentOrder(*this);
      }
       p->setState(PatientStatus::WAIT);
      switch (p->getCurrentTreatmentType()) 
      {
      case TreatmentType::ELECTRO:
        E_Waiting.insertSorted(p);
        break;
      case TreatmentType::ULTRASOUND:
        U_Waiting.insertSorted(p);
        break;
      case TreatmentType::EXERCISE:
        X_Waiting.insertSorted(p);
        break;
      }
    }
  }
}

void Scheduler::AssignX(int currentTime)
{
    int Xsize = X_Waiting.getCount();
    Patient* TempPatient;
    Treatment* t;
    for (int i = 0; i < Xsize; i++)
    {
        X_Waiting.dequeue(TempPatient);

        t = TempPatient->getCurrentTreatment();
        if (t->canAssign(*this))
        {
            Resource* room;
            X_Rooms.peek(room);
            room->incrementOcc();
            if (room->getOccupied() == room->getCapacity())
            {
                X_Rooms.dequeue(room);
            }
            t->assign(room, t->getDuration());
            TempPatient->startTreatment(currentTime);
            TempPatient->setState(PatientStatus::SERV);
            TempPatient->updateTT(t->getDuration());
            InTreatment.enqueue(TempPatient, -(currentTime + t->getDuration()));
        }
        else
            t->moveToWait(*this, TempPatient);
    }

}
void Scheduler::AssignU(int currentTime)
{
    int Usize = U_Waiting.getCount();
    Patient* TempPatient;
    Treatment* t;
    for (int i = 0; i < Usize; i++)
    {
        U_Waiting.dequeue(TempPatient);

        t = TempPatient->getCurrentTreatment();
        if (t->canAssign(*this))
        {
            InTreatment.enqueue(TempPatient, -(currentTime + t->getDuration()));
            Resource* ToAssign;
            U_Devices.dequeue(ToAssign);
            t->assign(ToAssign, t->getDuration());
            TempPatient->updateTT(t->getDuration());
            TempPatient->startTreatment(currentTime);
            TempPatient->setState(PatientStatus::SERV);
        }
        else
            t->moveToWait(*this, TempPatient);
    }

}
void Scheduler::AssignE(int currentTime)
{
    int Esize = E_Waiting.getCount();
    Patient* TempPatient;
    Treatment* t;
    for (int i = 0; i < Esize; i++)
    {
        E_Waiting.dequeue(TempPatient);

        t = TempPatient->getCurrentTreatment();
        if (t->canAssign(*this))
        {
            InTreatment.enqueue(TempPatient, -(currentTime + t->getDuration()));
            Resource* ToAssign;
            E_Devices.dequeue(ToAssign);
            t->assign(ToAssign, t->getDuration());
            TempPatient->updateTT(t->getDuration());
            TempPatient->startTreatment(currentTime);
            TempPatient->setState(PatientStatus::SERV);
        }
        else
            t->moveToWait(*this, TempPatient);
    }
}

int Scheduler::getLatency(Treatment *t) const {
  if (dynamic_cast<UltrasoundTreatment *>(t)) {
    return U_Waiting.calcTreatmentLatency();
  } else if (dynamic_cast<ElectroTreatment *>(t)) {
    return E_Waiting.calcTreatmentLatency();
  } else {
    return X_Waiting.calcTreatmentLatency();
  }
}

void Scheduler::UpdateInTreatment(int currentTime) 
{
  int size = InTreatment.getCount();
  for (int i = 0; i < size; ++i) 
  {
    Patient *p;
    int pri;
    InTreatment.dequeue(p, pri);
    if (currentTime - p->getTreatmentStartTime() >=p->getCurrentTreatmentDuration()) 
    {
        Treatment *t = p->getCurrentTreatment();
        Resource *res = t->getAssignedResource();
        if (res->getType() == TreatmentType::ELECTRO)
          E_Devices.enqueue(res);
        else if (res->getType() == TreatmentType::ULTRASOUND)
          U_Devices.enqueue(res);
        else 
        {
          if (res->getCapacity() == res->getOccupied())
            X_Rooms.enqueue(res);
          res->decrementOcc();
        }
        t->unassign();

        if (p->completeTreatment()) 
        {
          if (!p->hasMoreTreatments()) 
          {
            MoveToFinished(p);
          }
          else
          {
              p->setState(PatientStatus::WAIT);
              if (p->getType() == PatientType::RECOVERING)
              {
                  p->optimizeTreatmentOrder(*this);
              }
              switch (p->getCurrentTreatmentType())
              {
              case TreatmentType::ELECTRO:
                  E_Waiting.insertSorted(p);
                  break;
              case TreatmentType::ULTRASOUND:
                  U_Waiting.insertSorted(p);
                  break;
              case TreatmentType::EXERCISE:
                  X_Waiting.insertSorted(p);
                  break;
              }
          }
        } 
      }
    else 
    {
        if (currentTime - p->getTreatmentStartTime() < p->getCurrentTreatmentDuration()) 
        {
            int remainingTime = p->getCurrentTreatmentDuration() - (currentTime - p->getTreatmentStartTime());
            InTreatment.enqueue(p, -(currentTime + remainingTime));
        }
    }
  }
}

void Scheduler::MoveToFinished(Patient* p) 
{
  p->updateStatus(-1);
  p->setState(PatientStatus::FNSH);
  FinishedPatients.push(p);
}

bool Scheduler::hasAvailableUltrasoundDevice() { return !U_Devices.isEmpty(); }

void Scheduler::addToUWait(Patient *patient) {
  patient->setState(PatientStatus::WAIT);
  U_Waiting.insertSorted(patient);
}

bool Scheduler::hasAvailableExerciseRoom() 
{
    LinkedQueue<Resource*> tempRooms = X_Rooms;
    Resource* r;
    while (tempRooms.dequeue(r)) {
        if (r->getOccupied() < r->getCapacity())
            return true;
    }
    return false;
}


void Scheduler::addToXWait(Patient *patient) {
  patient->setState(PatientStatus::WAIT);
  X_Waiting.insertSorted(patient);
}

bool Scheduler::hasAvailableElectroDevice() 
{
    return !E_Devices.isEmpty();
}

void Scheduler::addToEWait(Patient *patient) {
  patient->setState(PatientStatus::WAIT);
  E_Waiting.insertSorted(patient);
}

void Scheduler::Cancel_And_Reshcudle()
{
    Patient* CancelPatient = X_Waiting.cancel();
    if (CancelPatient)
        MoveToFinished(CancelPatient);
    Patient* ReschedulePatient = EarlyPatients.reschedule();
    if (ReschedulePatient)
        EarlyPatients.enqueue(ReschedulePatient, -ReschedulePatient->getPT());   
}

void Scheduler::updateEarly_Late_list(int currentTime)
{
    Patient* TempPatient=NULL;
    int pri=0;
    int size=EarlyPatients.getCount();
    for(int i=0;i<size;i++)
        {
            EarlyPatients.dequeue(TempPatient,pri);
            if(TempPatient->getPT()==currentTime)
            {
                PatientType patientType = TempPatient->getType();
                if (patientType == PatientType::RECOVERING)
                  TempPatient->optimizeTreatmentOrder(*this);
                Treatment* t=TempPatient->getCurrentTreatment();
                TempPatient->setState(PatientStatus::WAIT);
                if(dynamic_cast<UltrasoundTreatment*>(t))
                    U_Waiting.enqueue(TempPatient);
                else if(dynamic_cast<ElectroTreatment*>(t))
                    E_Waiting.enqueue(TempPatient);
                else
                    X_Waiting.enqueue(TempPatient);
            }
            else
                EarlyPatients.enqueue(TempPatient,pri);
        }
    size=LatePatients.getCount();
    for(int i=0;i<size;i++)
        {
            LatePatients.dequeue(TempPatient,pri);
            if(currentTime== -pri)
            {
                PatientType patientType = TempPatient->getType();
                if (patientType == PatientType::RECOVERING)
                  TempPatient->optimizeTreatmentOrder(*this);
                Treatment* t=TempPatient->getCurrentTreatment();
                TempPatient->setState(PatientStatus::WAIT);
                if(dynamic_cast<UltrasoundTreatment*>(t))
                    U_Waiting.insertSorted(TempPatient);
                else if(dynamic_cast<ElectroTreatment*>(t))
                    E_Waiting.insertSorted(TempPatient);
                else
                    X_Waiting.insertSorted(TempPatient);
            }
            else
            {
                int prio = -(TempPatient->getLatePenalty() + TempPatient->getVT());
                LatePatients.enqueue(TempPatient, prio);

            }
        }
        
}
void Scheduler:: OutputFileMode(string fileName) {
  ofstream output(fileName);
  if (!output.is_open()) {
    cout << "can not open file";
    return;
  }
  output << "PID\tPType\tPT\tVT\tFT\tWT\tTT\tCancel\tResc\n";
  Patient *p;
  float Accepted_Cancel = 0, Accepted_Reschedule = 0, N_Total_Waiting = 0,
        R_Total_Waiting = 0, N_Total_Treatment = 0, R_Total_Treatment = 0,
        EarlyPatients = 0, LatePatients = 0, Total_Penalty = 0;
  ArrayStack<Patient *> TempFinishedPatients;
  while (!FinishedPatients.isEmpty()) {
    FinishedPatients.pop(p);
    output << p;
    if (p->getCancelled())
      Accepted_Cancel++;
    if (p->getRescheduled())
      Accepted_Reschedule++;
    if (p->getType() == PatientType::NORMAL) {
      N_Total_Waiting += p->getWT();
      N_Total_Treatment += p->getTT();
    } else {
      R_Total_Waiting += p->getWT();
      R_Total_Treatment += p->getTT();
    }

    if (p->getVT() < p->getPT())
      EarlyPatients++;
    else if (p->getVT() > p->getPT()) {
      LatePatients++;
      Total_Penalty += p->getLatePenalty();
    }
    TempFinishedPatients.push(p);
  }
  while (!TempFinishedPatients.isEmpty()) {
    TempFinishedPatients.pop(p);
    FinishedPatients.push(p);
  }
    FinishedPatients.peek(p);
  if(!p->getLatePenalty())
  output << "Total number of timesteps = " << p->getTT() + p->getPT() + p->getWT() << "\n";
  else
    output << "Total number of timesteps = " << p->getTT() + p->getVT() + p->getWT() + p->getLatePenalty() << "\n";
  output
      << "Total number of all, N, and R patients = "
      << FinishedPatients.GetCounter()<< ", " << Patient::get_NpatientsCount() << ", "
      << Patient::get_RpatientsCount() << "\n";

  output << "Average total waiting time for all, N, and R patients = "
         << (N_Total_Waiting + R_Total_Waiting)  / FinishedPatients.GetCounter()
         << ", " << ((Patient::get_NpatientsCount()) ? (N_Total_Waiting) / Patient::get_NpatientsCount() : 0)
         << ", " << ((Patient::get_RpatientsCount()) ? (R_Total_Waiting) / Patient::get_RpatientsCount() : 0 )
         << "\n";

  output << "Average total treatment time for all, N, and R patients = "
         << (N_Total_Treatment + R_Total_Treatment) / FinishedPatients.GetCounter()
         << ", " << ((Patient::get_NpatientsCount()) ? (N_Total_Treatment) / Patient::get_NpatientsCount() : 0)
         << ", " << ((Patient::get_RpatientsCount()) ? (R_Total_Treatment) / Patient::get_RpatientsCount() : 0)
         << "\n";

  output << "Percentage of patients of an accepted cancellation (%) = "
         << Accepted_Cancel * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Percentage of patients of an accepted rescheduling (%) = "
         << Accepted_Reschedule * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Percentage of early patients (%) = "
         << EarlyPatients * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Percentage of late patients (%) = "
         << LatePatients * 100 / FinishedPatients.GetCounter() << " %\n";
  
  output << "Average late penalty = " << ((LatePatients) ? (Total_Penalty / LatePatients) : 0) << "\n";
  

  
}

void Scheduler::incrementWaitingTime()
{
    Patient* p=NULL;
    
    int size=X_Waiting.getCount();
    for(int i=0;i<size;i++)
    {
        X_Waiting.dequeue(p);
        p->incrementWT();
        X_Waiting.enqueue(p);
    }
    
    size=U_Waiting.getCount();
    for(int i=0;i<size;i++)
    {
        U_Waiting.dequeue(p);
        p->incrementWT();
        U_Waiting.enqueue(p);
    }
    
    size=E_Waiting.getCount();
    for(int i=0;i<size;i++)
    {
         E_Waiting.dequeue(p);
         p->incrementWT();
         E_Waiting.enqueue(p);
    }
}