#pragma once
#include <iostream>
using namespace std;

enum class TreatmentType { ELECTRO, ULTRASOUND, EXERCISE };

class Resource {
protected:
    static int next_id;
    int id;
    TreatmentType type;
    int capacity;
    int Occupied;
public:
    Resource(TreatmentType t, int c = 1);
    int get_id();
    int getCapacity();
    TreatmentType getType();
    int getOccupied();
    void incrementOcc();
    void decrementOcc();
    friend ostream& operator <<(ostream& os,Resource*& resource);

};