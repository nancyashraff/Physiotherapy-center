#include "Treatment.h"
#include "Resource.h"
#include "Patient.h"
#include"Scheduler.h"
Treatment::Treatment(int d) : duration(d), assignmentTime(-1), assignedResource(nullptr)
{
}



Treatment::~Treatment()
{
}

int Treatment::getDuration() const
{
    return duration;
}

int Treatment::getAssignmentTime() const
{
    return assignmentTime;
}

Resource* Treatment::getAssignedResource() const
{
    return assignedResource;
}

void Treatment::assign(Resource* resource, int time)
{
    assignedResource = resource;
    assignmentTime = time;
}

void Treatment::unassign()
{
    assignedResource = nullptr;
    assignmentTime = -1;
}

void Treatment::setDuration(int d)
{
    duration = d;
}
