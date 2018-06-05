#include "wiringpiss.h"
#include <iostream>

using namespace std;

void wiringPiSetup()
{

}

void pinMode(int pin, int mode)
{
    cout << "pinMode, pin= " << pin << " mode= " << mode << endl;
}

void digitalWrite(int pin, int value)
{
    cout << "digitalWrite, pin= " << pin << " value= " << value << endl;
}
