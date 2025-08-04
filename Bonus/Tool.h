#pragma once

#include <iostream>
using namespace std;

class Tool
{
private:
    char Type;
public:
    Tool(char typ) 
    {
        Type = typ;
    }
    char getType() const 
        { return Type; }
};