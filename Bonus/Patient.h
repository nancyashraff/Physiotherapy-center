#ifndef PATIENT_H
#define PATIENT_H

#include"LinkedQueue.h"

#include"Resource.h"

class Scheduler;
class Treatment;
class ElectroTreatment;
class ExerciseTreatment;
class UltrasoundTreatment;
class Resource;

enum class PatientType { NORMAL, RECOVERING };
enum class PatientStatus { IDLE, ERLV, LATE, WAIT, SERV, FNSH };

class Patient {
protected:
    int PID;            // Patient ID
    PatientType type;
    int PT;             // Appointment Time
    int VT;             // Arrival Time
    PatientStatus status;
    LinkedQueue<Treatment*> requiredTreatments;
    int latePenalty;
    int currentTreatmentDuration;
    int treatmentStartTime;
    int WT;             // Waiting Time
    int TT;             // Treatment Time
    int LastTreatmentStartTime;
    int RescheduleCount;
    bool Cancelled;
    bool Rescheduled;
    bool Uinterrupt;
    bool Einterrupt;

    static int N_Patients;
    static int R_Patients;

public:
    Patient(int id, PatientType t, int appointmentTime, int arrivalTime, LinkedQueue<Treatment*>& treatments);

    void setPT(int newPT);
    void setState(PatientStatus p);
    void setTreatmentStartTime(int t);

    PatientType getType() { return type; }
    int getPID() const { return PID; }
    PatientType getType() const { return type; }
    int getPT() const { return PT; }
    int getVT() const { return VT; }
    int getWT() const { return WT; }
    int getTT() const { return TT; }
    PatientStatus getStatus() const { return status; }
    int getLatePenalty() const { return latePenalty; }
    int getCurrentTreatmentDuration() const { return currentTreatmentDuration; }
    int getTreatmentStartTime() const { return treatmentStartTime; }
    int getEffectivePriority() const { return PT + (isLate() ? latePenalty : 0); }

    bool isIdle() const { return status == PatientStatus::IDLE; }
    bool isEarly() const { return status == PatientStatus::ERLV; }
    bool isLate() const { return status == PatientStatus::LATE; }
    bool isWaiting() const { return status == PatientStatus::WAIT; }
    bool inTreatment() const { return status == PatientStatus::SERV; }
    bool isFinished() const { return status == PatientStatus::FNSH; }
    bool getCancelled() const { return Cancelled; }
    bool getRescheduled() const { return Rescheduled; }

    Treatment* getCurrentTreatment() const;
    TreatmentType getCurrentTreatmentType() const;
    bool hasMoreTreatments() const;
    bool isLastTreatmentXTherapy() const;
    void startTreatment(int currentTime);
    bool completeTreatment();
    void incrementRescheduleCount();
    int getRescheduleCount() const { return RescheduleCount; }
    

    // Special functions for recovering patients
    void optimizeTreatmentOrder(Scheduler& scheduler);

    void updateStatus(int currentTime);
    void calculateLatePenalty();

    int getTreatmentListSize() { return requiredTreatments.getCount(); }

    static int get_NpatientsCount() { return N_Patients; }
    static int get_RpatientsCount() { return R_Patients; }

    void updateTT(int duration);
    void incrementWT();
    void setCancelled();
    void setRescheduled();
    int getLastTreatmentStartTime() const;

    friend ostream& operator<<(ostream& os, const Patient* patient);

    void print(ostream& os) const;

    int ResourceID() const;

    friend ofstream& operator<<(ofstream& os, const Patient* patient);
    ~Patient();

    int getCurrentTreatmentDuration();

    void setUinterrupt();
    void setEinterrupt();
    bool getUinterrupt() const { return Uinterrupt; }
    bool getEinterrupt() const { return Einterrupt; }
    char ToolType() const;

};

#endif