
#pragma once
#include <iostream>
using namespace std;


enum class TreatmentType { ELECTRO, ULTRASOUND,  EXERCISE };


// Represents a treatment resource (device or room) in the physiotherapy center

class Resource {
protected:
    static int next_id;     // Static counter for generating unique resource IDs
    int id;                 // Unique identifier for the resource
    TreatmentType type;     // Type of treatment this resource supports

    // for X_Rooms only
    int capacity;           // Maximum number of patients that can use this resource
    int Occupied;           // Current number of patients using this resource

public:
    Resource(TreatmentType t, int c = 1);

    // Gets the unique identifier of the resource
    int get_id();

    // Gets the maximum capacity of the resource
    int getCapacity();

    // Gets the type of treatment this resource supports
    TreatmentType getType();

    // Gets the current number of patients using this resource(for X_Rooms only)
    int getOccupied();

    // Increments the occupancy counter when a patient starts using the resource(for X_Rooms only)
    void incrementOcc();

    // Decrements the occupancy counter when a patient finishes using the resource(for X_Rooms only)
    void decrementOcc();

    
    // Outputs the resource information 
    friend ostream& operator <<(ostream& os, Resource* resource);
};
