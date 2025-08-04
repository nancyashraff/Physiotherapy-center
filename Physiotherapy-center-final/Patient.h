
#ifndef PATIENT_H
#define PATIENT_H

#include "LinkedQueue.h"
#include "Resource.h"

class Scheduler;
class Treatment;
class ElectroTreatment;
class ExerciseTreatment;
class UltrasoundTreatment;
class Resource;

// Defines the types of patients that can be treated
enum class PatientType { NORMAL, RECOVERING };

// Represents the current status of a patient in the system

enum class PatientStatus {
  IDLE, // Patient has not yet arrived
  ERLV, // Patient arrived early
  LATE, // Patient arrived late
  WAIT, // Patient is waiting for treatment
  SERV, // Patient is currently being served
  FNSH  // Patient has finished all treatments
};

class Patient {
protected:
  int PID;              // Unique patient identifier
  PatientType type;     // Type of patient (NORMAL/RECOVERING)
  int PT;               // Appointment time
  int VT;               // Actual arrival time
  PatientStatus status; // Current patient status
  LinkedQueue<Treatment *> requiredTreatments; // Queue of treatments needed
  int latePenalty;                             // Penalty for late arrival
  int currentTreatmentDuration;                // Duration of current treatment
  int treatmentStartTime; // Start time of current treatment
  int WT;                 // Total waiting time
  int TT;                 // Total treatment time
  bool Cancelled;         // Whether patient was cancelled

  // rescheduled
  int RescheduleCount; // Number of times patient was rescheduled

  static int N_Patients; // Counter for normal patients
  static int R_Patients; // Counter for recovering patients

public:
  Patient(int id, PatientType t, int appointmentTime, int arrivalTime,
          LinkedQueue<Treatment *> &treatments);

  // Setters
  void setPT(int newPT);             // Sets new appointment time
  void setState(PatientStatus p);    // Updates patient status
  void setTreatmentStartTime(int t); // Sets treatment start time

  // Getters
  PatientType getType() { return type; }       // Returns patient type
  int getPID() const { return PID; }           // Returns patient ID
  PatientType getType() const { return type; } // Returns patient type
  int getPT() const { return PT; }             // Returns appointment time
  int getVT() const { return VT; }             // Returns actual arrival time
  int getWT() const { return WT; }             // Returns total waiting time
  int getTT() const { return TT; }             // Returns total treatment time
  PatientStatus getStatus() const {
    return status;
  } // Returns current patient status
  int getLatePenalty() const { return latePenalty; } // Returns late penalty
  int getCurrentTreatmentDuration() const {
    return currentTreatmentDuration;
  } // Returns current treatment duration
  int getTreatmentStartTime() const {
    return treatmentStartTime;
  } // Returns treatment start time
  int getEffectivePriority() const {
    return PT + (isLate() ? latePenalty : 0);
  } // Returns effective priority for late patients
  int getRescheduleCount() const { return RescheduleCount; }

  // Status checks
  bool isIdle() const { return status == PatientStatus::IDLE; }
  bool isEarly() const { return status == PatientStatus::ERLV; }
  bool isLate() const { return status == PatientStatus::LATE; }
  bool isWaiting() const { return status == PatientStatus::WAIT; }
  bool inTreatment() const { return status == PatientStatus::SERV; }
  bool isFinished() const { return status == PatientStatus::FNSH; }
  bool getCancelled() const { return Cancelled; }
  bool getRescheduled() const { return (RescheduleCount > 0); }

  // Treatment management
  Treatment *getCurrentTreatment() const;
  TreatmentType getCurrentTreatmentType() const;
  bool hasMoreTreatments() const;
  bool isLastTreatmentXTherapy() const;
  void startTreatment(int currentTime);
  bool completeTreatment();

  // Optimizes treatment order for recovering patients
  void optimizeTreatmentOrder(Scheduler &scheduler);

  void updateStatus(int currentTime);
  void calculateLatePenalty();
  int getTreatmentListSize() { return requiredTreatments.getCount(); }

  static int get_NpatientsCount() {
    return N_Patients;
  } // Returns count of normal patients
  static int get_RpatientsCount() {
    return R_Patients;
  } // Returns count of recovering patients

  // Update methods
  void updateTT(int duration);
  void incrementWT();
  void setCancelled();
  void incrementRescheduleCount();

  // Output operators
  friend ostream &operator<<(ostream &os, const Patient *patient);
  friend ofstream &operator<<(ofstream &os, const Patient *patient);
  void print(ostream &os) const;

  int ResourceID() const;
  int getCurrentTreatmentDuration();

  ~Patient();
};

#endif
