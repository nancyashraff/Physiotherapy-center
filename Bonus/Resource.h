#pragma once
#include <iostream>
#include "LinkedQueue.h"
#include "Tool.h"
using namespace std;

enum class TreatmentType { ELECTRO, ULTRASOUND, EXERCISE  };

class Resource {
protected:
    static int next_id;
    int id;
    TreatmentType type;
    int capacity;
    int Occupied;

    // Bonus Members
    LinkedQueue<Tool*> toolA_list;
    LinkedQueue<Tool*> toolB_list;
    LinkedQueue<Tool*> toolC_list;
    bool FreeFailure;
    bool BusyFailure;
    int toolA;
    int toolB;
    int toolC;
    char AssignedTool;

public:
    Resource(TreatmentType t, int c = 1, int toola = 0, int toolb = 0, int toolc= 0);
    int get_id();
    int getCapacity();
    TreatmentType getType();
    int getOccupied();
    void incrementOcc();
    void decrementOcc();
    friend ostream& operator <<(ostream& os,Resource* resource);

    // Bonus Methods
    void SetFreeFailure();
    void SetBusyFailure();
    bool getFreeFailure();   
    bool getBusyFailure();

    bool hasToolA();
    bool hasToolB();
    bool hasToolC();

    void addToolA(int count);
    void addToolB(int count);
    void addToolC(int count);

    void removeToolA();
    void removeToolB();
    void removeToolC();

    void setAssignedTool(char tool);
    char getAssignedTool();
    char getToolType() const { return AssignedTool; }
};