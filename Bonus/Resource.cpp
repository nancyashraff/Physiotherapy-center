#include "Resource.h"
// #include "Tool.h"

int Resource::next_id = 1;

Resource::Resource(TreatmentType t, int c, int toola, int toolb, int toolc)
{
    capacity = c;
    id = next_id;
    next_id++;
    type = t;
    Occupied=0;
    // Bonus
    FreeFailure=false;
    BusyFailure=false;
    toolA = toola;
    toolB = toolb;
    toolC = toolc;
    
    for (int i = 0; i < toolA; i++) 
        toolA_list.enqueue(new Tool('A'));
    for (int i = 0; i < toolB; i++) 
        toolB_list.enqueue(new Tool('B'));
    for (int i = 0; i < toolC; i++) 
        toolC_list.enqueue(new Tool('C'));
}

int Resource::get_id()
{
    return id;
}

int Resource::getCapacity()
{
    return capacity;
}

ostream& operator<<(ostream& os, Resource* resource) 
{
    if(resource->getType() == TreatmentType::EXERCISE)
        os << "R" << resource->get_id() << "[" << resource->getOccupied() << ", " << resource->getCapacity() << "]";
    else
        os <<resource->get_id() ;
    return os;
}
TreatmentType Resource:: getType()
{
    return type;
}
void Resource::incrementOcc()
{
    Occupied++;
}

void Resource::decrementOcc()
{
    Occupied--;
}

int Resource::getOccupied()
{
    return Occupied;
}

// Bonus Methods
void Resource::SetFreeFailure()
{
    FreeFailure=true;
}

void Resource::SetBusyFailure()
{
    BusyFailure=true;
}

bool Resource::getFreeFailure()
{
    return FreeFailure;
}

bool Resource::getBusyFailure()
{
    return BusyFailure;
}

bool Resource::hasToolA() 
{
    return !toolA_list.isEmpty(); 
}
bool Resource::hasToolB() 
{
    return !toolB_list.isEmpty(); 
}
bool Resource::hasToolC() 
{
    return !toolC_list.isEmpty(); 
}

void Resource::addToolA(int count) 
{
    for (int i = 0; i < count; ++i) 
        toolA_list.enqueue(new Tool('A')); 
}
void Resource::addToolB(int count) 
{
    for (int i = 0; i < count; ++i) 
        toolB_list.enqueue(new Tool('B')); 
}
void Resource::addToolC(int count) {
    for (int i = 0; i < count; ++i) 
        toolC_list.enqueue(new Tool('C')); 
}

void Resource::removeToolA() 
{
    
    if (!toolA_list.isEmpty())
    {
        Tool *del = NULL;
        toolA_list.dequeue(del); 
        delete del;
    }
        
}
void Resource::removeToolB() 
{
    if (!toolA_list.isEmpty())
        {
            Tool *del = NULL;
            toolB_list.dequeue(del); 
            delete del;
        } 
}
void Resource::removeToolC() 
{
    if (!toolA_list.isEmpty())
        {
            Tool *del = NULL;
            toolC_list.dequeue(del); 
            delete del;
        }
}

void Resource::setAssignedTool(char tool)
{
    AssignedTool=tool;
}

char Resource::getAssignedTool()
{
    if(AssignedTool=='A'||AssignedTool=='B'||AssignedTool=='C')
        return AssignedTool;
    else 
        return 'D';
}