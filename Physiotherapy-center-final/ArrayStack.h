#pragma once

#include "iostream"
using namespace std;
#define STACK_SIZE 200
template <typename T>
class ArrayStack // Dont change this
{
protected:
  T items[STACK_SIZE]; // Array of stack items
  int top;             // Index to top of stack
  int Counter;

public:
  ArrayStack() : Counter(0) { top = -1; } // end default constructor

  // Function getCapacity() returns the stack max size
  // added for array implementaion only
  int getCapacity() { return STACK_SIZE; }

  bool isEmpty() const { return top == -1; } // end isEmpty

  bool push(const T &newEntry) {
    if (top == STACK_SIZE - 1)
      return false; // Stack is FULL

    top++;
    items[top] = newEntry;
    Counter++;
    return true;
  } // end push

  bool pop(T &TopEntry) {
    if (isEmpty())
      return false;

    TopEntry = items[top];
    top--;
    Counter--;
    return true;
  } // end pop

  bool peek(T &TopEntry) const {
    if (isEmpty())
      return false;

    TopEntry = items[top];
    return true;
  } // end peek

  // Copy constructor
  ArrayStack(const ArrayStack<T> &S) {
    for (int i = 0; i <= S.top; i++)
      items[i] = S.items[i];
    top = S.top;
  }

  int getCount() const { return top + 1; }

  void print_stack() const {
    for (int i = top; i >= 0; i--) {
      cout << items[i] << " ";
    }
    cout << endl;
  }

  int GetCounter() const { return Counter; }

}; // end ArrayStack
