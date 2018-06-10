#include "pincontrolpi.h"
#include "globalsettings.h"

#ifndef RUNONPC
#include <wiringPi.h>
#else
#include "wiringpiss.h"
#endif

PinControlPi::PinControlPi()
{
    wiringPiSetup();
    pinMode(DECMINUS, OUTPUT);
    pinMode(DECPLUS, OUTPUT);
    pinMode(ASCMINUS, OUTPUT);
    pinMode(ASCPLUS, OUTPUT);
    pinMode(IVL1FOCUS, OUTPUT);
    pinMode(IVL1SHOOT, OUTPUT);
    pinMode(IVL2FOCUS, OUTPUT);
    pinMode(IVL2SHOOT, OUTPUT);
}

PinControlPi::~PinControlPi()
{
}

void PinControlPi::CameraFocus(int intervalometerNo)
{
    switch (intervalometerNo)
    {
    case 1:
        digitalWrite (IVL1FOCUS,1);
        break;
    case 2:
        digitalWrite (IVL2FOCUS,1);
        break;
    default:
        break;
    }
}

void PinControlPi::CameraShoot(int intervalometerNo)
{
    switch (intervalometerNo)
    {
    case 1:
        digitalWrite (IVL1SHOOT,1);
        break;
    case 2:
        digitalWrite (IVL2SHOOT,1);
        break;
    default:
        break;
    }
}

void PinControlPi::CameraRelease(int intervalometerNo)
{
    switch (intervalometerNo)
    {
    case 1:
        digitalWrite (IVL1SHOOT,0);
        digitalWrite (IVL1FOCUS,0);
        break;
    case 2:
        digitalWrite (IVL2SHOOT,0);
        digitalWrite (IVL2FOCUS,0);
        break;
    default:
        break;
    }
}

void PinControlPi::DeclinationPlus()
{
   digitalWrite(DECPLUS,1);
}

void PinControlPi::DeclinationMinus()
{
    digitalWrite(DECMINUS,1);
}

void PinControlPi::DeclinationRelease()
{
    digitalWrite(DECPLUS,0);
    digitalWrite(DECMINUS,0);
}

void PinControlPi::AscensionPlus()
{
    digitalWrite(ASCPLUS,1);
}

void PinControlPi::AscensionMinus()
{
    digitalWrite(ASCMINUS,1);
}

void PinControlPi::AscensionRelease()
{
    digitalWrite(ASCPLUS,0);
    digitalWrite(ASCMINUS,0);
}


