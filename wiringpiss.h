#ifndef WIRINGPISS_H
#define WIRINGPISS_H

#define OUTPUT 1

void wiringPiSetup();
void pinMode(int pin, int mode);
void digitalWrite(int pin, int value);

#endif // WIRINGPISS_H
