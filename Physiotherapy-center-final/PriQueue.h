#pragma once

#include"iostream"
using namespace std;
template < typename T>
class priNode
{
protected:
    T item;		// A data item
    int pri;	//priority of the item
    priNode<T>* next; // Pointer to next node
public:
    priNode(const T& r_Item, int PRI)
    {
        setItem(r_Item, PRI);
        next = nullptr;
    }
    void setItem(const T& r_Item, int PRI)
    {
        item = r_Item;
        pri = PRI;
    }
    void setNext(priNode<T>* nextNodePtr)
    {
        next = nextNodePtr;
    }

    T getItem(int& PRI) const
    {
        PRI = pri;
        return item;
    }

    priNode<T>* getNext() const
    {
        return next;
    }

    int getPri() const
    {
        return pri;
    }
}; // end Node

//This class impelements the priority queue as a sorted list (Linked List)
//The item with highest priority is at the front of the queue
template <typename T>
class PriQueue
{
    priNode<T>* head;
    int Counter;
public:
    PriQueue() : head(nullptr), Counter(0) {}

    ~PriQueue() {
        T tmp;
        int p;
        while (dequeue(tmp, p));
    }

    //insert the new node in its correct position according to its priority
    void enqueue(const T& data, int priority) {
        priNode<T>* newNode = new priNode<T>(data, priority);
        Counter++;
        if (head == nullptr || priority > head->getPri()) {

            newNode->setNext(head);
            head = newNode;
            return;
        }

        priNode<T>* current = head;
        while (current->getNext() && priority <= current->getNext()->getPri()) {
            current = current->getNext();
        }
        newNode->setNext(current->getNext());
        current->setNext(newNode);
    }

    bool dequeue(T& topEntry, int& pri) {
        if (isEmpty())
            return false;
        Counter--;
        topEntry = head->getItem(pri);
        priNode<T>* temp = head;
        head = head->getNext();
        delete temp;
        return true;
    }

    bool peek(T& topEntry, int& pri) {
        if (isEmpty())
            return false;

        topEntry = head->getItem(pri);
        pri = head->getPri();
        return true;
    }

    bool isEmpty() const {
        return head == nullptr;
    }
    int getCount() const
    {
        return Counter;
    }

    void print_list() const
    {
        int pri;
        priNode<T>* current = head;
        while (current)
        {
            cout << current->getItem(pri) << "  ";
            current = current->getNext();
        }
        cout << endl;
    }

};
