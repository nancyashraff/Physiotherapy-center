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

Scheduler::Scheduler() { srand(time(nullptr)); }

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

  for(int i=0;i<NG;i++)
    {
      int Acount, Bcount, Ccount;
      file >> Acount >> Bcount >> Ccount;
      Resource *res =NULL;
      X_Rooms.dequeue(res);
      res->addToolA(Acount);
      res->addToolB(Bcount);
      res->addToolC(Ccount);
      X_Rooms.enqueue(res);
    }
 
  int Pcancel, Presc;
  file >> Pcancel >> Presc;
  X_Waiting.SetPcancel(Pcancel);
  EarlyPatients.SetPresc(Presc);
  int FrPb, BsPb, MT;
  // For U_Devices
  file >> FrPb >> BsPb >> MT;
  U_FreeFailure = FrPb;
  U_BusyFailure = BsPb;
  U_MaintenanceDuration = MT;

  // For E_Devices
  file >> FrPb >> BsPb >> MT;
  E_FreeFailure = FrPb;
  E_BusyFailure = BsPb;
  E_MaintenanceDuration = MT;

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
      file >> treatTypeChar;
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

      
      Treatment *newTreatment = nullptr;
      switch (treatTypeChar) {
      case 'E':
        file >> duration;
        newTreatment = new ElectroTreatment(duration);
        hasElectro = true;
        break;
      case 'U':
        file >> duration;
        newTreatment = new UltrasoundTreatment(duration);
        hasUltrasound = true;
        break;
      case 'X':
        int da, db, dc;
        file >> da >> db >> dc;
        newTreatment = new ExerciseTreatment(da,db,dc);
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

void Scheduler::InitializeResources(int NE, int NU, int NG,const int *gymCapacities) {
  for (int i = 0; i < NE; ++i)
    E_Devices.enqueue(new Resource(TreatmentType::ELECTRO));

  for (int i = 0; i < NU; ++i)
    U_Devices.enqueue(new Resource(TreatmentType::ULTRASOUND));

  for (int i = 0; i < NG; ++i)
    X_Rooms.enqueue(new Resource(TreatmentType::EXERCISE, gymCapacities[i]));
  delete[] gymCapacities;
}

void Scheduler::RunSimulation() {
  int mode = UIobj.ReadInterfaceMode();
  if (mode > 2 || mode < 1) {
    cout << "Invalid Mode\n";
    return;
  }
  int currentTime = 0;
  cout << "Enter input file name: ";
  string FileName = UIobj.getFilename();
  LoadPatientsFromFile(FileName);
  if (mode == 1)
    while (!ALLPatients.isEmpty() || !InTreatment.isEmpty() ||
           !EarlyPatients.isEmpty() || !LatePatients.isEmpty() ||
           !E_Waiting.isEmpty() || !U_Waiting.isEmpty() ||
           !X_Waiting.isEmpty() || !E_InterruptPatients.isEmpty() || 
           !U_InterruptPatients.isEmpty() ) {

      HandleArrivals(currentTime);
      updateEarly_Late_list(currentTime);
      UpdateInTreatment(currentTime);
      UpdateInterruptPatient(currentTime);
      UpdateMaintenance(currentTime);
      AssignX(currentTime);
      AssignU(currentTime);
      AssignE(currentTime);
      incrementWaitingTime();
      Cancel_And_Reshcudle();

      UIobj.printConsole(currentTime, &ALLPatients, &EarlyPatients,
                         &LatePatients, &E_Waiting, &U_Waiting, &X_Waiting,
                         &InTreatment, &FinishedPatients, &E_Devices,
                         &U_Devices, &X_Rooms, &E_InterruptPatients, &U_InterruptPatients, &Maintenance);
      currentTime++;
      cout << "\nPress any key to display next timestep..." << endl;
      if (cin.get())
        continue;
    }
  else {
    cout << "Silent Mode, Simulation ends, Output file to be created\n";
    while (!ALLPatients.isEmpty() || !InTreatment.isEmpty() ||
           !EarlyPatients.isEmpty() || !LatePatients.isEmpty() ||
           !E_Waiting.isEmpty() || !U_Waiting.isEmpty() ||
           !X_Waiting.isEmpty()) {

      HandleArrivals(currentTime);
      updateEarly_Late_list(currentTime);
      UpdateInTreatment(currentTime);
      UpdateInterruptPatient(currentTime);
      AssignX(currentTime);
      AssignU(currentTime);
      AssignE(currentTime);
      UpdateMaintenance(currentTime);
      incrementWaitingTime();
      Cancel_And_Reshcudle();

      currentTime++;
    }
  }
  cout << "Enter output file name: ";
  FileName = UIobj.getFilename();
  OutputFileMode(FileName);
  cout << "Output file created successfully.\n";
}

void Scheduler::HandleArrivals(int currentTime) {
  if (!currentTime)
    return;
  int size = ALLPatients.getCount();
  for (int i = 0; i < size; ++i) {
    Patient *p;
    ALLPatients.dequeue(p);
    if (p->getVT() > currentTime) {
      ALLPatients.enqueue(p);
      p->setState(PatientStatus::IDLE);
    } else if (p->getVT() < p->getPT()) {
      EarlyPatients.enqueue(p, -(p->getPT()));
      p->setState(PatientStatus::ERLV);
    } else if (p->getVT() > p->getPT()) {
      p->calculateLatePenalty();
      int prio = -(p->getLatePenalty() + p->getVT()); // get priority
      LatePatients.enqueue(p, prio);
      p->setState(PatientStatus::LATE);
    } else {
      if (p->getType() == PatientType::RECOVERING) {
        p->optimizeTreatmentOrder(*this);
      }
      p->setState(PatientStatus::WAIT);
      switch (p->getCurrentTreatmentType()) {
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

void Scheduler::AssignX(int currentTime) {
  int Xsize = X_Waiting.getCount();
  Patient *TempPatient;
  Treatment *t;
  for (int i = 0; i < Xsize; i++) {
    X_Waiting.dequeue(TempPatient);
    t = TempPatient->getCurrentTreatment();
    ExerciseTreatment *X=dynamic_cast<ExerciseTreatment *>(t);
    if (t->canAssign(*this)) {
      Resource *room;
      X_Rooms.peek(room);
      room->incrementOcc();
      if (room->getOccupied() == room->getCapacity()) {
        X_Rooms.dequeue(room);
      }
      if(X->getHas_A()&&room->hasToolA())
      {
        t->assign(room, X->getA_Duration());
        TempPatient->updateTT(X->getA_Duration());
        room->removeToolA();
        room->setAssignedTool('A');
        InTreatment.enqueue(TempPatient, -(currentTime + X->getA_Duration()));
        TempPatient->startTreatment(currentTime);
        TempPatient->setState(PatientStatus::SERV);
      }
      else if(X->getHas_B()&&room->hasToolB())
      {
        t->assign(room, X->getB_Duration());
        TempPatient->updateTT(X->getB_Duration());
        room->removeToolB();
        room->setAssignedTool('B');
        InTreatment.enqueue(TempPatient, -(currentTime + X->getB_Duration()));
        TempPatient->startTreatment(currentTime);
        TempPatient->setState(PatientStatus::SERV);
      }
      else if(X->getHas_C()&&room->hasToolC())
      {
        t->assign(room, X->getC_Duration());
        TempPatient->updateTT(X->getC_Duration());
        room->removeToolC();
        room->setAssignedTool('C');
        InTreatment.enqueue(TempPatient, -(currentTime + X->getC_Duration()));
        TempPatient->startTreatment(currentTime);
        TempPatient->setState(PatientStatus::SERV);
      }
      else
        X_Waiting.enqueue(TempPatient);
    } else
      t->moveToWait(*this, TempPatient);
  }
}
void Scheduler::AssignU(int currentTime) 
{
  int Usize = U_Waiting.getCount();
  Patient *TempPatient;
  Treatment *t;
  for (int i = 0; i < Usize; i++) {
    U_Waiting.dequeue(TempPatient);

    t = TempPatient->getCurrentTreatment();
    if (t->canAssign(*this)) {
      if (rand() % 100 < U_FreeFailure) {
        Resource *Umaintaince = NULL;
        U_Devices.dequeue(Umaintaince);
        Umaintaince->SetFreeFailure();
        Maintenance.enqueue(Umaintaince, -(currentTime + U_MaintenanceDuration));
        U_Waiting.enqueue(TempPatient);
      } else {
        InTreatment.enqueue(TempPatient, -(currentTime + t->getDuration()));
        Resource *ToAssign;
        U_Devices.dequeue(ToAssign);
        t->assign(ToAssign, t->getDuration());
        TempPatient->updateTT(t->getDuration());
        TempPatient->startTreatment(currentTime);
        TempPatient->setState(PatientStatus::SERV);
      }
    } else
      t->moveToWait(*this, TempPatient);
  }
}
void Scheduler::AssignE(int currentTime) {
  int Esize = E_Waiting.getCount();
  Patient *TempPatient;
  Treatment *t;
  for (int i = 0; i < Esize; i++) {
    E_Waiting.dequeue(TempPatient);

    t = TempPatient->getCurrentTreatment();
    if (t->canAssign(*this)) {
      if (rand() % 100 < E_FreeFailure) {
        Resource *Emaintaince = NULL;
        E_Devices.dequeue(Emaintaince);
        Emaintaince->SetFreeFailure();
        Maintenance.enqueue(Emaintaince, -(currentTime + E_MaintenanceDuration));
        E_Waiting.enqueue(TempPatient);
      } else {
        InTreatment.enqueue(TempPatient, -(currentTime + t->getDuration()));
        Resource *ToAssign;
        E_Devices.dequeue(ToAssign);
        t->assign(ToAssign, t->getDuration());
        TempPatient->updateTT(t->getDuration());
        TempPatient->startTreatment(currentTime);
        TempPatient->setState(PatientStatus::SERV);
      }
    } else
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

void Scheduler::UpdateInTreatment(int currentTime) {
  int size = InTreatment.getCount();
  for (int i = 0; i < size; ++i) {
    Patient *p;
    int pri;
    InTreatment.dequeue(p, pri);
    if (currentTime - p->getTreatmentStartTime() >= p->getCurrentTreatmentDuration()) {
      Treatment *t = p->getCurrentTreatment();
      Resource *res = t->getAssignedResource();
      ExerciseTreatment *X=dynamic_cast<ExerciseTreatment *>(t);
      if (res->getType() == TreatmentType::ELECTRO)
        E_Devices.enqueue(res);
      else if (res->getType() == TreatmentType::ULTRASOUND)
        U_Devices.enqueue(res);
      else {
        if (res->getCapacity() == res->getOccupied())
          X_Rooms.enqueue(res);
        res->decrementOcc();
        if(res->getAssignedTool()=='A'){
          res->addToolA(1);
          res->setAssignedTool('D');
          X->A_is_Done();
        }
        else if(res->getAssignedTool()=='B'){
          res->addToolB(1);
          res->setAssignedTool('D');
          X->B_is_Done();
        }
        else{
          res->addToolC(1);
          res->setAssignedTool('D');
          X->C_is_Done();
        }
      }
      t->unassign();
      if(dynamic_cast<ExerciseTreatment *>(t))
        if (!(X->getHas_A()||X->getHas_B()||X->getHas_C())) 
          p->completeTreatment();
      p->completeTreatment();
        if (!p->hasMoreTreatments()) {
          MoveToFinished(p);
        } else {
          p->setState(PatientStatus::WAIT);
          if (p->getType() == PatientType::RECOVERING) {
            p->optimizeTreatmentOrder(*this);
          }
          switch (p->getCurrentTreatmentType()) {
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
      
    } else {
      if (currentTime - p->getTreatmentStartTime() < p->getCurrentTreatmentDuration()) {
        int remainingTime = p->getCurrentTreatmentDuration() - (currentTime - p->getTreatmentStartTime());
        Treatment *t = p->getCurrentTreatment();
        static bool seeded = false;
        if (!seeded) {
          srand(static_cast<unsigned>(time(nullptr)));
          seeded = true;
        }
        if (p->getCurrentTreatmentType() == TreatmentType::ULTRASOUND) {
          if (rand() % 100 >= U_BusyFailure)
            InTreatment.enqueue(p, -(currentTime + remainingTime));
          else {
            Resource *Umaintaince = t->getAssignedResource();
            Umaintaince->SetBusyFailure();
            Maintenance.enqueue(Umaintaince, -(currentTime + U_MaintenanceDuration));
            t->setDuration(remainingTime);
            U_InterruptPatients.enqueue(p);
            p->setState(PatientStatus :: WAIT);
            p->setUinterrupt();
          }
        } else if (p->getCurrentTreatmentType() == TreatmentType::ELECTRO) {
          if (rand() % 100 >= E_BusyFailure)
            InTreatment.enqueue(p, -(currentTime + remainingTime));
          else {
            Resource *Emaintaince = t->getAssignedResource();
            Emaintaince->SetBusyFailure();
            Maintenance.enqueue(Emaintaince, -(currentTime + E_MaintenanceDuration));
            t->setDuration(remainingTime);
            E_InterruptPatients.enqueue(p);
            p->setState(PatientStatus ::WAIT);
            p->setEinterrupt();
          }
        } else
          InTreatment.enqueue(p, -(currentTime + remainingTime));
      }
    }
  }
}

void Scheduler::MoveToFinished(Patient *p) {
  p->updateStatus(-1);
  p->setState(PatientStatus::FNSH);
  FinishedPatients.push(p);
}

bool Scheduler::hasAvailableUltrasoundDevice() { return !U_Devices.isEmpty(); }

void Scheduler::addToUWait(Patient *patient) {
  patient->setState(PatientStatus::WAIT);
  U_Waiting.insertSorted(patient);
}

bool Scheduler::hasAvailableExerciseRoom() {
  LinkedQueue<Resource *> tempRooms = X_Rooms;
  Resource *r;
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

bool Scheduler::hasAvailableElectroDevice() { return !E_Devices.isEmpty(); }

void Scheduler::addToEWait(Patient *patient) {
  patient->setState(PatientStatus::WAIT);
  E_Waiting.insertSorted(patient);
}

void Scheduler::Cancel_And_Reshcudle() {
  Patient *CancelPatient = X_Waiting.cancel();
  if (CancelPatient)
    MoveToFinished(CancelPatient);
  Patient *ReschedulePatient = EarlyPatients.reschedule();
  if (ReschedulePatient)
    EarlyPatients.enqueue(ReschedulePatient, -ReschedulePatient->getPT());
}

void Scheduler::updateEarly_Late_list(int currentTime) {
  Patient *TempPatient = NULL;
  int pri = 0;
  int size = EarlyPatients.getCount();
  for (int i = 0; i < size; i++) {
    EarlyPatients.dequeue(TempPatient, pri);
    if (TempPatient->getPT() == currentTime) {
      PatientType patientType = TempPatient->getType();
      if (patientType == PatientType::RECOVERING)
        TempPatient->optimizeTreatmentOrder(*this);
      Treatment *t = TempPatient->getCurrentTreatment();
      TempPatient->setState(PatientStatus::WAIT);
      if (dynamic_cast<UltrasoundTreatment *>(t))
        U_Waiting.enqueue(TempPatient);
      else if (dynamic_cast<ElectroTreatment *>(t))
        E_Waiting.enqueue(TempPatient);
      else
        X_Waiting.enqueue(TempPatient);
    } else
      EarlyPatients.enqueue(TempPatient, pri);
  }
  size = LatePatients.getCount();
  for (int i = 0; i < size; i++) {
    LatePatients.dequeue(TempPatient, pri);
    if (currentTime == -pri) {
      PatientType patientType = TempPatient->getType();
      if (patientType == PatientType::RECOVERING)
        TempPatient->optimizeTreatmentOrder(*this);
      Treatment *t = TempPatient->getCurrentTreatment();
      TempPatient->setState(PatientStatus::WAIT);
      if (dynamic_cast<UltrasoundTreatment *>(t))
        U_Waiting.insertSorted(TempPatient);
      else if (dynamic_cast<ElectroTreatment *>(t))
        E_Waiting.insertSorted(TempPatient);
      else
        X_Waiting.insertSorted(TempPatient);
    } else {
      int prio = -(TempPatient->getLatePenalty() + TempPatient->getVT());
      LatePatients.enqueue(TempPatient, prio);
    }
  }
}
void Scheduler::OutputFileMode(string fileName) {
  ofstream output(fileName);
  if (!output.is_open()) {
    cout << "can not open file";
    return;
  }
  output << "PID\tPType\tPT\tVT\tFT\tWT\tTT\tCancel\tResc\n";
  Patient *p;
  float Accepted_Cancel = 0, Accepted_Reschedule = 0, N_Total_Waiting = 0,
        R_Total_Waiting = 0, N_Total_Treatment = 0, R_Total_Treatment = 0,
        EarlyPatients = 0, LatePatients = 0, Total_Penalty = 0 ,Uint=0, Eint=0;
  // Bonus
  float E_FreeFail=0,E_BusyFail=0;
  float U_FreeFail=0,U_BusyFail=0;
  
  ArrayStack<Patient *> TempFinishedPatients;
  while (!FinishedPatients.isEmpty()) {
    FinishedPatients.pop(p);
    output << p;
    if (p->getCancelled())
      Accepted_Cancel++;
    if (p->getRescheduled())
      Accepted_Reschedule++;
    if(p->getUinterrupt())
      Uint++;
    if(p->getEinterrupt())
      Eint++;
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
  int size=E_Devices.getCount();
  Resource *res=NULL;
  while(size--)
    {
      E_Devices.dequeue(res);
      if(res->getFreeFailure())
        E_FreeFail++;
      if(res->getBusyFailure())
        E_BusyFail++;
      E_Devices.enqueue(res);
    }
  size=U_Devices.getCount();
  while(size--)
    {
      U_Devices.dequeue(res);
      if(res->getFreeFailure())
        U_FreeFail++;
      if(res->getBusyFailure())
        U_BusyFail++;
      U_Devices.enqueue(res);
    }
  
  FinishedPatients.peek(p);
  if (!p->getLatePenalty())
    output << "Total number of timesteps = "
           << p->getTT() + p->getPT() + p->getWT() << "\n";
  else
    output << "Total number of timesteps = "
           << p->getTT() + p->getVT() + p->getWT() + p->getLatePenalty()
           << "\n";
  output << "Total number of all, N, and R patients = "
         << FinishedPatients.GetCounter() << ", "
         << Patient::get_NpatientsCount() << ", "
         << Patient::get_RpatientsCount() << "\n";

  output << "Average total waiting time for all, N, and R patients = "
         << (N_Total_Waiting + R_Total_Waiting) / FinishedPatients.GetCounter()
         << ", " << (N_Total_Waiting) / Patient::get_NpatientsCount() << ", "
         << (R_Total_Waiting) / Patient::get_RpatientsCount() << "\n";

  output << "Average total treatment time for all, N, and R patients = "
         << (N_Total_Treatment + R_Total_Treatment) /
                FinishedPatients.GetCounter()
         << ", " << (N_Total_Treatment) / Patient::get_NpatientsCount() << ", "
         << (R_Total_Treatment) / Patient::get_RpatientsCount() << "\n";

  output << "Percentage of patients of an accepted cancellation (%) = "
         << Accepted_Cancel * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Percentage of patients of an accepted rescheduling (%) = "
         << Accepted_Reschedule * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Percentage of early patients (%) = "
         << EarlyPatients * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Percentage of late patients (%) = "
         << LatePatients * 100 / FinishedPatients.GetCounter() << " %\n";

  output << "Average late penalty = " << Total_Penalty / LatePatients << "\n\n";

  output << "Bonus Modifications:\n";

  output << "Percentage of interrupt for All, Electro & Ultrasonic devices (%) = " << ((Uint+Eint)*100/FinishedPatients.GetCounter())  <<" %, "<< (Eint*100/FinishedPatients.GetCounter()) <<" %, "<< (Uint*100/FinishedPatients.GetCounter()) << " %\n";

  output <<"\nDevices Percentage of Failure:\n";
  output << "Percentage of Free failure for All, Electro & Ultrasonic devices (%) = "
    <<(E_FreeFail+U_FreeFail)*100/(E_Devices.getCount()+U_Devices.getCount())<<" %, "
    << E_FreeFail*100/E_Devices.getCount()<<" %, "<<U_FreeFail*100/U_Devices.getCount()<<" %\n";

  output << "Percentage of Busy failure for All, Electro & Ultrasonic devices (%) = "
    <<(E_BusyFail+U_BusyFail)*100/(E_Devices.getCount()+U_Devices.getCount())<<" %, "
    << E_BusyFail*100/E_Devices.getCount()<<" %, "<<U_BusyFail*100/U_Devices.getCount()<<" %\n";
}

void Scheduler::incrementWaitingTime() {
  Patient *p = NULL;

  int size = X_Waiting.getCount();
  for (int i = 0; i < size; i++) {
    X_Waiting.dequeue(p);
    p->incrementWT();
    X_Waiting.enqueue(p);
  }

  size = U_Waiting.getCount();
  for (int i = 0; i < size; i++) {
    U_Waiting.dequeue(p);
    p->incrementWT();
    U_Waiting.enqueue(p);
  }

  size = E_Waiting.getCount();
  for (int i = 0; i < size; i++) {
    E_Waiting.dequeue(p);
    p->incrementWT();
    E_Waiting.enqueue(p);
  }
}

void Scheduler::UpdateMaintenance(int currentTime) {
  int size = Maintenance.getCount();
  while (size--) {
    Resource *res;
    int pri;
    Maintenance.dequeue(res, pri);
    if (-pri > currentTime) {
      Maintenance.enqueue(res, pri);
      return;
    }
    if (currentTime == -pri) {
      if (res->getType() == TreatmentType::ELECTRO)
        E_Devices.enqueue(res);
      else
        U_Devices.enqueue(res);
    }
  }
}

void Scheduler::UpdateInterruptPatient(int currentTime) {
  int size = U_InterruptPatients.getCount();
  while (size--) {
    Patient *p;
    U_InterruptPatients.dequeue(p);
    Treatment *t = p->getCurrentTreatment();
    if (t->canAssign(*this)) {
      InTreatment.enqueue(p, -(currentTime + t->getDuration()));
      Resource *ToAssign;
      U_Devices.dequeue(ToAssign);
      t->assign(ToAssign, t->getDuration());
      p->setState(PatientStatus::SERV);
    } else
      U_InterruptPatients.enqueue(p);
  }

  size = E_InterruptPatients.getCount();
  while (size--) {
    Patient *p;
    E_InterruptPatients.dequeue(p);
    Treatment *t = p->getCurrentTreatment();
    if (t->canAssign(*this)) {
      InTreatment.enqueue(p, -(currentTime + t->getDuration()));
      Resource *ToAssign;
      E_Devices.dequeue(ToAssign);
      t->assign(ToAssign, t->getDuration());
      p->setState(PatientStatus::SERV);
    } else
      E_InterruptPatients.enqueue(p);
  }
}
