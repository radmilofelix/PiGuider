#ifndef PINCONTROLPI_H
#define PINCONTROLPI_H

#define DECPLUS 29
#define DECMINUS 26
#define ASCPLUS 27
#define ASCMINUS 28

#define IVL1FOCUS 6
#define IVL1SHOOT 4

#define IVL2FOCUS 1
#define IVL2SHOOT 15

#define IVL1 1
#define IVL2 2


class PinControlPi
{
public:
    PinControlPi();
    void CameraFocus(int intervalometerNo);
    void CameraShoot(int intervalometerNo);
    void CameraRelease(int intervalometerNo);
    void DeclinationPlus();
    void DeclinationMinus();
    void DeclinationRelease();
    void AscensionPlus();
    void AscensionMinus();
    void AscensionRelease();
};

#endif // PINCONTROLPI_H
