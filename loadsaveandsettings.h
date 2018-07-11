#ifndef LOADSAVEANDSETTINGS_H
#define LOADSAVEANDSETTINGS_H

#include <QWidget>
#include "globalsettings.h"

namespace Ui {
class LoadSaveAndSettings;
}


//                   params
//====================================================
// string
//#define MAXCHARPARAMS 0

// double
#define paramRaSlope PARAMSDOUBLE[0]
#define MAXDOUBLEPARAMS 1

// float
#define paramAlpha PARAMSFLOAT[0]
#define paramArcsecPerPixel PARAMSFLOAT[1]
#define paramsMinThreshold PARAMSFLOAT[2]
#define paramsThresholdStep PARAMSFLOAT[3]
#define paramsMaxThreshold PARAMSFLOAT[4]
#define paramsMinArea PARAMSFLOAT[5]
#define paramsMaxArea PARAMSFLOAT[6]
#define MAXFLOATPARAMS 7

// long
#define hwJobDelayPeriod1 PARAMSLONG[0]
#define hwRestPeriod1 PARAMSLONG[1]
#define hwFocusPeriod1 PARAMSLONG[2]
#define hwExposurePeriod1 PARAMSLONG[3]
#define hwJobDelayPeriod2 PARAMSLONG[4]
#define hwRestPeriod2 PARAMSLONG[5]
#define hwFocusPeriod2 PARAMSLONG[6]
#define hwExposurePeriod2 PARAMSLONG[7]
#define swJobDelayPeriod1 PARAMSLONG[8]
#define swRestPeriod1 PARAMSLONG[9]
#define swFocusPeriod1 PARAMSLONG[10]
#define swExposurePeriod1 PARAMSLONG[11]
#define timeDrift PARAMSLONG[12]
#define MAXLONGPARAMS 13

// integer
#define paramSlopeVertical PARAMSINTEGER[0]
#define paramCalibrationStatus PARAMSINTEGER[1]
#define hwIterations1 PARAMSINTEGER[2]
#define hwIterations2 PARAMSINTEGER[3]
#define swIterations1 PARAMSINTEGER[4]
#define MAXINTEGERPARAMS 5

//                    settings
//====================================================

// string
#define usbFlashMemoryPath SETTINGSCHAR[0]
#define sdFlashMemoryPath SETTINGSCHAR[1]
#define GuiderFolder SETTINGSCHAR[2]
#define DSLRFolder SETTINGSCHAR[3]
#define MAXCHARSETTINGS 4

// integer
#define setRelativeWidth SETTINGSINTEGER[0]
#define setRelativeHeight SETTINGSINTEGER[1]
#define setTriggerGuide SETTINGSINTEGER[2] // = 2 arcsec
#define captureTethered SETTINGSINTEGER[3]
#define captureToUSB    SETTINGSINTEGER[4]
#define iSDeclinationTracked SETTINGSINTEGER[5]
#define captureJustGuiderData SETTINGSINTEGER[6]
#define MAXINTEGERSETTINGS 7

//float
#define setGuidingSpeed SETTINGSFLOAT[0] // (0.5X-StarAdventurer) (0.5X-iEQ30Pro default)
#define MAXFLOATSETTINGS 1

//double
#define setNormalTrackingSpeed SETTINGSDOUBLE[0] //arcsecs/second - this is a constant equal to earth rotation speed = 15 arcsec/second
#define MAXDOUBLESETTINGS 1

/*
struct paramsChar
{
    char name[40];
    char value[200];
};
*/

struct paramsDouble
{
    char name[40];
    double value;
};

struct paramsFloat
{
    char name[40];
    float value;
};

struct paramsLong
{
    char name[40];
    long value;
};

struct paramsInteger
{
    char name[40];
    int value;
};

struct settingsChar
{
    char name[40];
    char value[100];
};

struct settingsInteger
{
    char name[40];
    int value;
};

struct settingsFloat
{
    char name[40];
    float value;
};

struct settingsDouble
{
    char name[40];
    double value;
};

class LoadSaveAndSettings : public QWidget
{
    Q_OBJECT

public:
    explicit LoadSaveAndSettings(QWidget *parent = 0);
    ~LoadSaveAndSettings();
//    paramsChar PARAMSCHAR[MAXCHARPARAMS];
    paramsDouble PARAMSDOUBLE[MAXDOUBLEPARAMS];
    paramsFloat PARAMSFLOAT[MAXFLOATPARAMS];
    paramsLong PARAMSLONG[MAXLONGPARAMS];
    paramsInteger PARAMSINTEGER[MAXINTEGERPARAMS];
    settingsChar SETTINGSCHAR[MAXCHARSETTINGS];
    settingsInteger SETTINGSINTEGER[MAXINTEGERSETTINGS];
    settingsFloat SETTINGSFLOAT[MAXFLOATSETTINGS];
    settingsDouble SETTINGSDOUBLE[MAXDOUBLESETTINGS];

    char parameterName[50];
    char parameterValue[20];
    char numericParameterStringBuffer[50];
    QString chronologicFilename;
    bool stopOnSettingsError;

    int ClearSpaces(char* buf, int dim, char delimiter, bool onlyAroundDelimiter);
    void DispatchErrorMessage(char* errmess);
    int CheckData(bool readConfiguration);
    int CheckAndSetValidData();
    int ReadSettings();
    int ReadParams();
    void DisplayData();
	void MakeInvalidParamsZero();
    int SaveParams();
    void SetChronologicFileName();
    int IsFile(char* filename);
    bool IsUSBFolder();
    bool IsSDFolder();
    int CreateFolders(char* folderName);
    int CanWriteToFolder(char* folder);
    
private:
    Ui::LoadSaveAndSettings *ui;
    int ReadData(char* configurationOrParams, char* defaultParams, bool readConfiguration);
};

#endif // LOADSAVEANDSETTINGS_H
