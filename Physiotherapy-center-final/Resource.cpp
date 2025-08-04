#include "Resource.h"

int Resource::next_id = 1;

Resource::Resource(TreatmentType t, int c)
{
	capacity = c;
	id = next_id;
	next_id++;
	type = t;
    Occupied=0;
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
