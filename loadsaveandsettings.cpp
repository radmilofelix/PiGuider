#include "loadsaveandsettings.h"
#include "ui_loadsaveandsettings.h"
//#include <stdio.h>
//#include <stdlib.h>
#include <unistd.h>

///////
#include <iostream>
using namespace std;
/////////

#define INVALIDVALUE -31415926


//static const char* paramsCharNamesToMatch[] =
//{
//};


static const char* paramsIntegerNamesToMatch[] =
{
    "paramSlopeVertical",
    "paramCalibrationStatus",
    "hwIterations1",
    "hwIterations2",
    "swIterations1"
};

static const char* paramsLongNamesToMatch[] =
{
    "hwJobDelayPeriod1",
    "hwRestPeriod1",
    "hwFocusPeriod1",
    "hwExposurePeriod1",
    "hwJobDelayPeriod2",
    "hwRestPeriod2",
    "hwFocusPeriod2",
    "hwExposurePeriod2",
    "swJobDelayPeriod1",
    "swRestPeriod1",
    "swFocusPeriod1",
    "swExposurePeriod1",
    "timeDrift"
};

static const char* paramsFloatNamesToMatch[] =
{
    "paramAlpha",
    "paramArcsecPerPixel",
    "paramsMinThreshold",
    "paramsThresholdStep",
    "paramsMaxThreshold",
    "paramsMinArea",
    "paramsMaxArea"
};

static const char* paramsDoubleNamesToMatch[] =
{
    "paramRaSlope"
};

static const char* settingsCharNamesToMatch[] =
{
    "usbFlashMemoryPath",
    "sdFlashMemoryPath",
    "GuiderFolder",
    "DSLRFolder"
};

static const char* settingsIntegerNamesToMatch[] =
{
    "setRelativeWidth",
    "setRelativeHeight",
    "setTriggerGuide",
    "captureTethered",
    "captureToUSB",
    "iSDeclinationTracked",
    "captureJustGuiderData"
};

static const char* settingsFloatNamesToMatch[] =
{
    "setGuidingSpeed"
};

static const char* settingsDoubleNamesToMatch[] =
{
    "setNormalTrackingSpeed"
};

LoadSaveAndSettings::LoadSaveAndSettings(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::LoadSaveAndSettings)
{
    ui->setupUi(this);
    stopOnSettingsError=false;
    int i;

/*
    for(i=0; i<MAXCHARPARAMS; i++)
    {
        strcpy(PARAMSCHAR[i].name, paramsCharNamesToMatch[i]);
        strcpy(PARAMSCHAR[i].value, "");
    }
*/

    for(i=0; i<MAXCHARSETTINGS; i++)
    {
        strcpy(SETTINGSCHAR[i].name, settingsCharNamesToMatch[i]);
        strcpy(SETTINGSCHAR[i].value, "");
    }

    for(i=0; i<MAXDOUBLEPARAMS; i++)
    {
        strcpy(PARAMSDOUBLE[i].name,paramsDoubleNamesToMatch[i]);
        PARAMSDOUBLE[i].value=INVALIDVALUE;
    }

    for(i=0; i<MAXFLOATPARAMS; i++)
    {
        strcpy(PARAMSFLOAT[i].name,paramsFloatNamesToMatch[i]);
        PARAMSFLOAT[i].value=INVALIDVALUE;
    }

    for(i=0; i<MAXLONGPARAMS; i++)
    {
        strcpy(PARAMSLONG[i].name,paramsLongNamesToMatch[i]);
        PARAMSLONG[i].value=INVALIDVALUE;
    }

    for(i=0; i<MAXINTEGERPARAMS; i++)
    {
        strcpy(PARAMSINTEGER[i].name,paramsIntegerNamesToMatch[i]);
        PARAMSINTEGER[i].value=INVALIDVALUE;
    }

    for(i=0; i<MAXINTEGERSETTINGS; i++)
    {
        strcpy(SETTINGSINTEGER[i].name,settingsIntegerNamesToMatch[i]);
        SETTINGSINTEGER[i].value=INVALIDVALUE;
    }

    for(i=0; i<MAXFLOATSETTINGS; i++)
    {
        strcpy(SETTINGSFLOAT[i].name,settingsFloatNamesToMatch[i]);
        SETTINGSFLOAT[i].value=INVALIDVALUE;
    }

    for(i=0; i<MAXDOUBLESETTINGS; i++)
    {
        strcpy(SETTINGSDOUBLE[i].name,settingsDoubleNamesToMatch[i]);
        SETTINGSDOUBLE[i].value=INVALIDVALUE;
    }
    if(!ReadSettings())
        stopOnSettingsError=true;
    ReadParams();
    if(!CreateFolders(GuiderFolder.value))
        stopOnSettingsError=true;
    if(!CreateFolders(DSLRFolder.value))
        stopOnSettingsError=true;

    if(IsUSBFolder())
    {
        if(!CanWriteToFolder(usbFlashMemoryPath.value))
        {
            cout << "Can not write to USB!" << endl;
            stopOnSettingsError=true;
        }
        char folderName[200];
        strcpy(folderName,usbFlashMemoryPath.value);
        strcat(folderName, GuiderFolder.value);
        if(!CanWriteToFolder(folderName))
        {
            cout << "Can not write to USB Guider folder!" << endl;
            stopOnSettingsError=true;
        }
        strcpy(folderName,usbFlashMemoryPath.value);
        strcat(folderName, DSLRFolder.value);
        if(!CanWriteToFolder(folderName))
        {
            cout << "Can not write to USB DSLR folder!" << endl;
            stopOnSettingsError=true;
        }
    }
    if(!CanWriteToFolder(sdFlashMemoryPath.value))
    {
        cout << "Can not write to user folder on SD!" << endl;
        stopOnSettingsError=true;
    }
    char folderName[200];
    strcpy(folderName,sdFlashMemoryPath.value);
    strcat(folderName, GuiderFolder.value);
    if(!CanWriteToFolder(folderName))
    {
        cout << "Can not write to user Guider folder on SD!" << endl;
        stopOnSettingsError=true;
    }
    strcpy(folderName,sdFlashMemoryPath.value);
    strcat(folderName, DSLRFolder.value);
    if(!CanWriteToFolder(folderName))
    {
        cout << "Can not write to user DSLR folder on SD!" << endl;
        stopOnSettingsError=true;
    }
}

LoadSaveAndSettings::~LoadSaveAndSettings()
{
    delete ui;
}

int LoadSaveAndSettings::ClearSpaces(char* buf, int dim, char delimiter, bool onlyAroundDelimiter)
{
    // Clear spaces in definition line.
    // If a delimiter character is used, the text after the delimiter will not
    // be processed if "onlyAroundDelimiter" is true.
    if(delimiter==' ') return dim;
    int i,j,multi;
    bool delimiterFound=false;
    bool clearPermit=true;
    multi=1;
    for(i=0; i<dim; i++)
    {
        if( *(buf+i)==delimiter)
        {
            delimiterFound=true;
            if( *(buf+i+1) != ' ')
                clearPermit=false;
        }
        if ( *(buf+i)==' ' && clearPermit )
        {
            while( *(buf+i+multi)==' ')
                multi++;
            for(j=i;j<dim-multi;j++)
                *(buf+j)=*(buf+j+multi);
            dim-=multi;
            multi=1;
            if(onlyAroundDelimiter && delimiterFound)
                clearPermit=false;
            if( *(buf+i)==delimiter)
            {
                delimiterFound=true;
                if( *(buf+i+1) != ' ')
                    clearPermit=false;
            }
        }
    }
    return dim;
}

void LoadSaveAndSettings::DispatchErrorMessage(char* errmess)
{
    cout << "Error in configuration or data file. Could not read the value of " << errmess << endl;
}

int LoadSaveAndSettings::CheckData(bool readConfiguration)
{
    if(!readConfiguration)
    {
        for(int i=0;i<MAXDOUBLEPARAMS;i++)
        {
            if( PARAMSDOUBLE[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)PARAMSDOUBLE[i].name);
                return 0;
            }
        }

        for(int i=0;i<MAXFLOATPARAMS;i++)
        {
            if( PARAMSFLOAT[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)PARAMSFLOAT[i].name);
                return 0;
            }
        }

        for(int i=0;i<MAXLONGPARAMS;i++)
        {
            if( PARAMSLONG[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)PARAMSLONG[i].name);
                return 0;
            }
        }

        for(int i=0;i<MAXINTEGERPARAMS;i++)
        {
            if( PARAMSINTEGER[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)PARAMSINTEGER[i].name);
                return 0;
            }
        }
    }
    else
    {
        for(int i=0;i<MAXINTEGERSETTINGS;i++)
        {
            if( SETTINGSINTEGER[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)SETTINGSINTEGER[i].name);
                return 0;
            }
        }

        for(int i=0;i<MAXFLOATSETTINGS;i++)
        {
            if( SETTINGSFLOAT[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)SETTINGSFLOAT[i].name);
                return 0;
            }
        }

        for(int i=0;i<MAXDOUBLESETTINGS;i++)
        {
            if( SETTINGSDOUBLE[i].value==INVALIDVALUE )
            {
                DispatchErrorMessage((char*)SETTINGSDOUBLE[i].name);
                return 0;
            }
        }

        for(int i=0;i<MAXCHARSETTINGS;i++)
        {
            if(strlen(SETTINGSCHAR[i].value)==0)
            {
                DispatchErrorMessage((char*)SETTINGSCHAR[i].name);
                return 0;
            }
        }
    }
    return 1;
}

int LoadSaveAndSettings::CheckAndSetValidData()
{
    QString buff=parameterValue;
/*
    for(int i=0;i<MAXCHARPARAMS;i++)
    {
        if(!strcmp(parameterName, PARAMSCHAR[i].name))
        {
            strcpy(PARAMSCHAR[i].value, parameterValue);
            return 1;
        }
    }
*/
    for(int i=0;i<MAXDOUBLEPARAMS;i++)
    {
        if(!strcmp(parameterName, PARAMSDOUBLE[i].name))
        {
            PARAMSDOUBLE[i].value=buff.toDouble();
            return 1;
        }
    }

    for(int i=0;i<MAXFLOATPARAMS;i++)
    {
        if(!strcmp(parameterName, PARAMSFLOAT[i].name))
        {
            PARAMSFLOAT[i].value=buff.toFloat();
            return 1;
        }
    }

    for(int i=0;i<MAXLONGPARAMS;i++)
    {
        if(!strcmp(parameterName, PARAMSLONG[i].name))
        {
            strcpy(numericParameterStringBuffer, parameterValue);
            PARAMSLONG[i].value=atol(numericParameterStringBuffer);
            return 1;
        }
    }

    for(int i=0;i<MAXINTEGERPARAMS;i++)
    {
        if(!strcmp(parameterName, PARAMSINTEGER[i].name))
        {
            strcpy(numericParameterStringBuffer, parameterValue);
            PARAMSINTEGER[i].value=atoi(numericParameterStringBuffer);
            return 1;
        }
    }

    for(int i=0;i<MAXCHARSETTINGS;i++)
    {
        if(!strcmp(parameterName, SETTINGSCHAR[i].name))
        {
            strcpy(SETTINGSCHAR[i].value, parameterValue);
            return 1;
        }
    }

    for(int i=0;i<MAXINTEGERSETTINGS;i++)
    {
        if(!strcmp(parameterName, SETTINGSINTEGER[i].name))
        {
            strcpy(numericParameterStringBuffer, parameterValue);
            SETTINGSINTEGER[i].value=atoi(numericParameterStringBuffer);
            return 1;
        }
    }

    for(int i=0;i<MAXFLOATSETTINGS;i++)
    {
        if(!strcmp(parameterName, SETTINGSFLOAT[i].name))
        {
            SETTINGSFLOAT[i].value=buff.toFloat();
            return 1;
        }
    }

    for(int i=0;i<MAXDOUBLESETTINGS;i++)
    {
        if(!strcmp(parameterName, SETTINGSDOUBLE[i].name))
        {
            SETTINGSDOUBLE[i].value=buff.toDouble();
            return 1;
        }
    }
    return 0;
}

int LoadSaveAndSettings::ReadData(char* configurationOrParams, char* defaultParams, bool readConfiguration)
{
    // Initialise the configuration variables by reading them from the
    // configuration file.
    int i;
    FILE* mycfg;
    char *line = NULL;
    char parameter[512];
    size_t len = 0;
    ssize_t read;
    if(readConfiguration)
    {
        mycfg=fopen(configurationOrParams, "r");
        if (mycfg == NULL)
        {
            cout << "Configuration file not found. Abandoning read." << endl;
            return 0;
        }
    }
    else
    {
        char fileName[200];
        if(IsUSBFolder())
        {
            strcpy(fileName, usbFlashMemoryPath.value);
            strcat(fileName, configurationOrParams);
            mycfg=fopen(fileName, "r");
            if (mycfg == NULL)
            {
                cout << "Parameters file not found on USB. Loadig file from program folder..." << endl;
                mycfg=fopen(configurationOrParams, "r");
                if (mycfg == NULL)
                {
                    cout << "Default parameters file not found. Loadig default file..." << endl;
                    mycfg=fopen(defaultParams, "r");
                    if (mycfg == NULL)
                    {
                        cout << "Default parameters file not found. Abandoning read." << endl;
                        return 0;
                    }
                }
            }
        }
        else
        {
            mycfg=fopen(configurationOrParams, "r");
            if (mycfg == NULL)
            {
                cout << "Parameters file not found. Loadig default file..." << endl;
                mycfg=fopen(defaultParams, "r");
                if (mycfg == NULL)
                {
                    cout << "Default parameters file not found. Abandoning read." << endl;
                    return 0;
                }
            }
        }
    }
    while ((read = getline(&line, &len, mycfg)) != -1)
    {
        line[read-1]=0;
        ClearSpaces(line, read,'=',true);
        strcpy(parameter,line);

        // blank lines, lines starting with "#", ";", or space will be ignored
        if( read>1 && parameter[0]!='#' && parameter[0]!=';' && parameter[0]!=' ')
        {
            for(i=0; i<read; i++)
            {
                // Check parameter/value delimiter. If you want the delimiter
                // to be the space character, replace '=' with ' ',
                // comment-out the clearspaces function above and take
                // care not to have spaces in the configuration file
                // before and after the parameter name.
                if(parameter[i]=='=')
                {
                    parameter[i]=0;
                    strcpy(parameterName,parameter);
                    strcpy(parameterValue,parameter+i+1);
                    if(!CheckAndSetValidData()) // set the value to the paramenter variable if parameter is valid
                    {
                        cout << "Bad settings parameter: " << parameterName << endl;
                    }
                    break;
                }
            }
        }
    }
    free(line);
    fclose(mycfg);
    if( !CheckData(readConfiguration) ) // check if all the configuration variables were set, exits if not
        return 0;
    return 1;
}

int LoadSaveAndSettings::ReadSettings()
{
    return ReadData("PiGuider-settings.cfg","", true);
}

int LoadSaveAndSettings::ReadParams()
{
    return ReadData("PiGuider-params.cfg", "PiGuider-params-default.cfg",false);
}

void LoadSaveAndSettings::DisplayData()
{
#ifdef DEBUG
    cout << fixed;
    cout << endl << "Configuration data read from file..." << endl;
    cout << " ======== PARAMETERS ========" << endl;
    for(int i=0;i<MAXINTEGERPARAMS;i++)
        cout << PARAMSINTEGER[i].name << " = " << PARAMSINTEGER[i].value << endl;
    cout << endl;
    for(int i=0;i<MAXLONGPARAMS;i++)
        cout << PARAMSLONG[i].name << " = " << PARAMSLONG[i].value << endl;
    cout << endl;
    for(int i=0;i<MAXFLOATPARAMS;i++)
        cout << PARAMSFLOAT[i].name << " = " << PARAMSFLOAT[i].value << endl;
    cout << endl;
    for(int i=0;i<MAXDOUBLEPARAMS;i++)
        cout << PARAMSDOUBLE[i].name << " = " << PARAMSDOUBLE[i].value << endl;
    cout << endl;
    cout << endl << " ======== SETTINGS ========" << endl;
    for(int i=0;i<MAXCHARSETTINGS;i++)
        cout << SETTINGSCHAR[i].name << " = " << SETTINGSCHAR[i].value << endl;
    cout << endl;
    for(int i=0;i<MAXINTEGERSETTINGS;i++)
        cout << SETTINGSINTEGER[i].name << " = " << SETTINGSINTEGER[i].value << endl;
    cout << endl;
    for(int i=0;i<MAXFLOATSETTINGS;i++)
        cout << SETTINGSFLOAT[i].name << " = " << SETTINGSFLOAT[i].value << endl;
    cout << endl;
    for(int i=0;i<MAXDOUBLESETTINGS;i++)
        cout << SETTINGSDOUBLE[i].name << " = " << SETTINGSDOUBLE[i].value << endl;
    cout << endl;
#endif
}

void LoadSaveAndSettings::MakeInvalidParamsZero()
{
    for(int i=0;i<MAXINTEGERPARAMS;i++)
    {
        if( PARAMSINTEGER[i].value==INVALIDVALUE)
        {
            DispatchErrorMessage((char*)PARAMSINTEGER[i].name);
            PARAMSINTEGER[i].value=0;
        }
    }

    for(int i=0;i<MAXLONGPARAMS;i++)
    {
        if( PARAMSLONG[i].value==INVALIDVALUE)
        {
            DispatchErrorMessage((char*)PARAMSLONG[i].name);
            PARAMSLONG[i].value=0;
        }
    }

    for(int i=0;i<MAXFLOATPARAMS;i++)
    {
        if( PARAMSFLOAT[i].value==INVALIDVALUE)
        {
            DispatchErrorMessage((char*)PARAMSFLOAT[i].name);
            PARAMSFLOAT[i].value=0;
        }
    }

    for(int i=0;i<MAXDOUBLEPARAMS;i++)
    {
        if( PARAMSDOUBLE[i].value==INVALIDVALUE)
        {
            DispatchErrorMessage((char*)PARAMSDOUBLE[i].name);
            PARAMSDOUBLE[i].value=0;
        }
    }
return;
}

int LoadSaveAndSettings::SaveParams()
{
    // Save Parameers to file
    QString buff;
    FILE* parameterFile;
    if(IsUSBFolder())
    {
        char fileName[200];
        strcpy(fileName, usbFlashMemoryPath.value);
        strcat(fileName, "PiGuider-params.cfg");
        if( !(parameterFile=fopen(fileName, "w")))
        {
            cout << "Unable to open parameter file " << "params.cfg" << endl;
            stopOnSettingsError=true;
            return 0;
        }
    }
    else
    {
        if( !(parameterFile=fopen("PiGuider-params.cfg", "w")))
        {
            cout << "Unable to open parameter file " << "params.cfg" << endl;
            stopOnSettingsError=true;
            return 0;
        }
    }

    for(int i=0;i<MAXINTEGERPARAMS;i++)
    {
        buff=PARAMSINTEGER[i].name;
        buff+= "=" + QString::number((double)PARAMSINTEGER[i].value) + "\n";
        fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    }
    buff="\n";
    fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    for(int i=0;i<MAXLONGPARAMS;i++)
    {
        buff=PARAMSLONG[i].name;
        buff+= "=" + QString::number((double)PARAMSLONG[i].value) + "\n";
        fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    }
    buff="\n";
    fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    for(int i=0;i<MAXFLOATPARAMS;i++)
    {
        buff=PARAMSFLOAT[i].name;
        buff+= "=" + QString::number((double)PARAMSFLOAT[i].value) + "\n";
        fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    }
    buff="\n";
    fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    for(int i=0;i<MAXDOUBLEPARAMS;i++)
    {
        buff=PARAMSDOUBLE[i].name;
        buff+= "=" + QString::number((double)PARAMSDOUBLE[i].value) + "\n";
        fwrite(buff.toLatin1(), buff.length(), 1, parameterFile);
    }
    fclose(parameterFile);
    return 1;
}

void LoadSaveAndSettings::SetChronologicFileName()
{
    time_t currtime = time(&currtime);
    currtime+=timeDrift.value;
    struct tm * ptime;
    ptime=gmtime(&currtime); // UTC time
//    ptime=localtime(&currtime); // Local time
    chronologicFilename=QString::number(ptime->tm_year+1900) + "-";
    chronologicFilename+=QString::number(ptime->tm_mon+1).rightJustified(2, '0') + "-";
    chronologicFilename+=QString::number(ptime->tm_mday).rightJustified(2, '0') + "_";
    chronologicFilename+=QString::number(ptime->tm_hour).rightJustified(2, '0') + ".";
    chronologicFilename+=QString::number(ptime->tm_min).rightJustified(2, '0') + ".";
    chronologicFilename+=QString::number(ptime->tm_sec).rightJustified(2, '0');
}

int LoadSaveAndSettings::IsFile(char *filename)
{
    if( access( filename, F_OK ) != -1 )
        return 1;
    else
        return 0;
}

bool LoadSaveAndSettings::IsUSBFolder()
{
    if(IsFile(usbFlashMemoryPath.value))
        return true;
    else
        return false;
}

bool LoadSaveAndSettings::IsSDFolder()
{
    if(IsFile(sdFlashMemoryPath.value))
        return true;
    else
        return false;
}

int LoadSaveAndSettings::CreateFolders(char* folderName)
{
    char newFolder[200];
    if(IsUSBFolder())
    {
        strcpy(newFolder,usbFlashMemoryPath.value);
        strcat(newFolder, folderName);
        if(!IsFile(newFolder))
        {
            char systemCommand[200];
            strcpy(systemCommand, "mkdir ");
            strcat(systemCommand,newFolder);
            if(system(systemCommand)!=0)
            {
                cout << "Can not create USB folder: " << folderName <<endl;
                return 0;
            }
        }
    }
    if(!IsSDFolder())
    {
        cout << "User folder is absent. Creating user folder." << endl;
        char systemCommand[200];
        strcpy(systemCommand, "mkdir ");
        strcat(systemCommand,sdFlashMemoryPath.value);
        if(system(systemCommand)!=0)
        {
            cout << "Can not create user folder!" << endl;
            return 0;
        }
    }
    strcpy(newFolder,sdFlashMemoryPath.value);
    strcat(newFolder, folderName);
    if(!IsFile(newFolder))
    {
        char systemCommand[200];
        strcpy(systemCommand, "mkdir ");
        strcat(systemCommand,newFolder);
        if(system(systemCommand)!=0)
        {
            cout << "Can not create SD folder: " << folderName <<endl;
            return 0;
        }
    }
    return 1;
}

int LoadSaveAndSettings::CanWriteToFolder(char* folder)
{
    char systemCommand[200];
    strcpy(systemCommand, "mkdir ");
    strcat(systemCommand, folder);
    strcat(systemCommand, "testdir/");
    if(system(systemCommand))
        return 0;
    strcpy(systemCommand, folder);
    strcat(systemCommand, "testdir/");
    if( IsFile(systemCommand) )
    {
        strcpy(systemCommand, "rm -r ");
        strcat(systemCommand, folder);
        strcat(systemCommand, "testdir");
        system(systemCommand);
        return 1;
    }
    else
    {
        return 0;
    }
}

