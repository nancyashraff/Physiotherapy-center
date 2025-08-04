#ifndef EXERCISE_TREATMENT_H
#define EXERCISE_TREATMENT_H

#include"Treatment.h"

class Scheduler;
class Treatment;

class ExerciseTreatment : public Treatment {
private:
    // Bonus Members
    int A_Duration;
    int B_Duration;
    int C_Duration;
    bool has_A;
    bool has_B;
    bool has_C;
public:
    ExerciseTreatment(int dA,int dB,int dC) : Treatment(dA+dB+dC),A_Duration(dA), B_Duration(dB), C_Duration(dC)
    {
        (dA>0)? has_A=true:has_A=false;
        (dB>0)? has_B=true:has_B=false;
        (dC>0)? has_C=true:has_C=false;
    }

    bool canAssign(Scheduler& scheduler) const;
    void moveToWait(Scheduler& scheduler, Patient* patient);

    int getA_Duration() const { return A_Duration; }
    int getB_Duration() const { return B_Duration; }
    int getC_Duration() const { return C_Duration; }
    bool getHas_A() const { return has_A; }
    bool getHas_B() const { return has_B; }
    bool getHas_C() const { return has_C; }
    void A_is_Done() { has_A = false; }
    void B_is_Done() { has_B = false; }
    void C_is_Done() { has_C = false; }
};

#endif