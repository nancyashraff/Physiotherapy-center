#pragma once
#include <iostream>
#include <string>
#include <iomanip>
#include "LinkedQueue.h"
#include "PriQueue.h"
#include "ArrayStack.h"
#include "EarlyPList.h"
#include "EU_WaitList.h"
#include "Patient.h"
#include "Resource.h"
using namespace std;


class UI {
public:
    string getFilename() 
    {
        string filename;
        cin >> filename;
        return filename;
    }
    int ReadInterfaceMode()
    {
        cout<<"Enter interface mode(1 for Interactive Mode, 2 for silent mode): ";
        int mode;
        cin >> mode;
        return mode;
    }

    void printConsole(int& timestep,const LinkedQueue<Patient*>* allPatients,const EarlyPList* earlyList,const PriQueue<Patient*>* lateList,
        const EU_WaitList* eWait,const EU_WaitList* uWait,const LinkedQueue<Patient*>* xWait,const PriQueue<Patient*>* inTreatment,const ArrayStack<Patient*>* finished,
         LinkedQueue<Resource*>* E_Devices, LinkedQueue<Resource*>* U_Devices, LinkedQueue<Resource*>* X_Rooms) 
    {
        cout << "Current Timestep: " << timestep << "\n\n";

        printAllPatientsList(allPatients);

        printWaitingLists(eWait, uWait, xWait);

        printEarlyLateLists(earlyList, lateList);

        printAvailableResources(E_Devices, U_Devices, X_Rooms);

        printInTreatmentList(inTreatment);

        printFinishedPatients(finished);
    }

private:
    void printAllPatientsList(const LinkedQueue<Patient*>* list) 
    {
        cout << "ALL List\n";
        cout << list->getCount() << " patients remaining: ";
        if (list->getCount() <= 10)
        {
            list->print_list();
        }
        if (list->getCount() > 10)
        {
            Patient* p;
            LinkedQueue<Patient*>temp(*list);
            for (int i = 0; i < 10; i++)
            {
                temp.dequeue(p);
                cout << p;
            }
            cout << "....";
        }
        cout << "\n";
    }

    void printWaitingLists(const EU_WaitList* eWait,const EU_WaitList* uWait,const LinkedQueue<Patient*>* xWait) 
    {
        cout << "===================== Waiting Lists ======================\n";
        printSingleWaitList("E-therapy", eWait);
        printSingleWaitList("U-therapy", uWait);
        printSingleWaitList("X-therapy", xWait);
        cout << "\n";
    }

    void printSingleWaitList(const string& name, const EU_WaitList* list) 
    {
        cout << list->getCount() << " " << name << " patients: ";
        list->print_list();
        cout << "\n";
    }

    void printSingleWaitList(const string& name, const LinkedQueue<Patient*>* list) 
    {
        cout << list->getCount() << " " << name << " patients: ";

        list->print_list();
        cout << "\n";
    }

    void printEarlyLateLists(const EarlyPList* early, const PriQueue<Patient*>* late) 
    {
        cout << "===================== Early List ======================\n";
        cout << early->getCount() << " patients: ";
        early->print_list();

        cout << "===================== Late List ======================\n";
        cout << late->getCount() << " patients: ";
        late->print_list();
        cout << "\n";
    }

    void printAvailableResources(LinkedQueue<Resource*>* E_Devices, LinkedQueue<Resource*>* U_Devices, LinkedQueue<Resource*>* X_Rooms) 
    {
        cout << "===================== Avail E-devices ======================\n";
        cout << E_Devices->getCount() << " Electro devices: ";

        E_Devices->print_list();
        cout << "\n";

        cout << "===================== Avail U-devices ======================\n";
        cout << U_Devices->getCount() << " Ultra devices: ";

        U_Devices->print_list();
        cout << "\n";

        cout << "===================== Avail X-rooms ======================\n";
        cout << X_Rooms->getCount() << " rooms: ";

        X_Rooms->print_list();
        cout << "\n\n";
    }

    void printInTreatmentList(const PriQueue<Patient*>* list) {
        cout << "===================== In-treatment List ======================\n";
        cout << list->getCount() << " patients: ";
        list->print_list();
        cout << "\n";
    }

    void printFinishedPatients(const ArrayStack<Patient*>* stack) 
    {
        std::cout << stack->getCount() << " finished patients: ";
        stack->print_stack();

        cout << "(Recently finished are printed first)\n\n";
    }

};
