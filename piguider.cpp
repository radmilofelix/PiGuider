#include "piguider.h"
#include "ui_piguider.h"
#include <QTimer>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "globalsettings.h"
#include "numpad.h"


//using namespace cv;
using namespace std;

PiGuider::PiGuider(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PiGuider)
{
    //  Remove main window frame
    this->setWindowFlags( Qt::FramelessWindowHint );
    ui->setupUi(this);
    connect(&numpad, SIGNAL(ReturnClickedSignal()), this, SLOT(NumpadReturnClicked()));
    dslr_status=false;
    guider_status=false;
    editEnabled=true;
    intervalometersoft_status=false;
    intervalometerhard_status=false;
    buttonSize.setHeight(70);
    buttonSize.setWidth(70);
    raGuideStatus=0;
    declGuideStatus=0;

//    QPixmap image("://media/icons/tools-16x16/led-red.png");
//    image.load("://media/icons/tools-16x16/led-red.png");
//    ui->ledLabelGuider->setPixmap(image);
//    ui->ledLabelDSLR->setPixmap(image);
//    ui->ledLabelSoftIntervalometer->setPixmap(image);
//    ui->ledLabelHardIntervalometer->setPixmap(image);
    image.load("://media/icons/tools-32x32/arrowLeftGrey.png");
    ui->leftLabel->setPixmap(image);
    image.load("://media/icons/tools-32x32/arrowRightGrey.png");
    ui->rightLabel->setPixmap(image);
    image.load("://media/icons/tools-32x32/arrowUpGrey.png");
    ui->upLabel->setPixmap(image);
    image.load("://media/icons/tools-32x32/arrowDownGrey.png");
    ui->downLabel->setPixmap(image);
    ui->iv1CyclesLabel->setText("0");
    ui->iv1SecondsLabel->setText("0");
    ui->iv2CyclesLabel->setText("0");
    ui->iv2SecondsLabel->setText("0");

    ReadTime();
    timerMain = new QTimer(this);

    buttonpressed=false;
    QObject::connect(timerMain, SIGNAL(timeout()), this, SLOT(UpdateTime()));
//    timerMain->start(1000);
    timerMain->start(500);
    readTimeCounter=50;
    if(lsas.stopOnSettingsError)
    {
        close();
        cout << "PiGuider can not continue.\nPress SPACE to exit or close program terminal window ." << endl;
        while (1)
        {
            if (' ' == getchar())
               break;
        }
        exit(0);
    }
}

PiGuider::~PiGuider()
{
    delete ui;
    delete timerMain;
}

void PiGuider::ReadValues()
{
    year=ui->yearLineEdit->text().toInt();
    month=ui->monthLineEdit->text().toInt();
    day=ui->dayLineEdit->text().toInt();
    hours=ui->hoursLineEdit->text().toInt();
    minutes=ui->minutesLineEdit->text().toInt();
}

void PiGuider::ValidateInput()
{
    if(month < 1)
    {
        month=1;
        ui->monthLineEdit->setText("01");
    }
    if(month > 12)
    {
        month=12;
        ui->monthLineEdit->setText("12");
    }
    if(day < 1)
    {
        day=1;
        ui->dayLineEdit->setText("01");
    }
    if(day > 31)
    {
        day=28;
        ui->dayLineEdit->setText("28");
    }
    if(hours < 0)
    {
        hours=0;
        ui->hoursLineEdit->setText("00");
    }
    if(hours > 23)
    {
        hours=0;
        ui->hoursLineEdit->setText("00");
    }
    if(minutes < 0)
    {
        minutes=0;
        ui->minutesLineEdit->setText("00");
    }
    if(minutes > 59)
    {
        minutes=59;
        ui->minutesLineEdit->setText("59");
    }
}

void PiGuider::ReadTime()
{
    time_t currtime = time(&currtime);
    struct tm * ptime;
    currtime+=lsas.timeDrift.value;
    ptime=gmtime(&currtime); // UTC time
//    ptime=localtime(&currtime); // Local time


    year=ptime->tm_year+1900;
    month=ptime->tm_mon+1;
    day=ptime->tm_mday;
    hours=ptime->tm_hour;
    minutes=ptime->tm_min;

    ui->yearLineEdit->setText(QString::number((int)year));
    if(month<10)
        ui->monthLineEdit->setText("0"+QString::number((int)month));
    else
        ui->monthLineEdit->setText(QString::number((int)month));

    if(day<10)
        ui->dayLineEdit->setText("0"+QString::number((int)day));
    else
        ui->dayLineEdit->setText(QString::number((int)day));

    if(hours<10)
        ui->hoursLineEdit->setText("0"+QString::number((int)hours));
    else
        ui->hoursLineEdit->setText(QString::number((int)hours));

    if(minutes<10)
        ui->minutesLineEdit->setText("0"+QString::number((int)minutes));
    else
        ui->minutesLineEdit->setText(QString::number((int)minutes));
}

void PiGuider::SetTime()
{
    time_t currentTime=time(0);
    struct tm ptime={0};
    ptime.tm_year=year-1900;
    ptime.tm_mon=month-1;
    ptime.tm_mday=day;
    ptime.tm_hour=hours;
    ptime.tm_min=minutes;
    ptime.tm_sec=0;
    time_t realTime = timegm(&ptime); // UTC time
//    time_t realTime = mktime(&ptime); // Local time
    lsas.timeDrift.value=realTime-currentTime;
    lsas.SaveParams();
}

void PiGuider::NumPadCaller(int nameOfControl)
{
    if(!numpad.busy) // && enableVirtualKeyboard)
    {
        numpad.callerName=nameOfControl;
        numpad.busy=true;
        numpad.inputResult="0";
        numpad.Refresh();
        numpad.show();
    }
    else
        NumpadReturnClicked();
}

void PiGuider::UpdateTime()
{
    UpdateStatus();
}

void PiGuider::UpdateStatus()
{
    QPixmap ledred("://media/icons/tools-16x16/led-red.png");
    QPixmap ledgreen("://media/icons/tools-16x16/led-green.png");

    if(!readTimeCounter)
        readTimeCounter--;
    else
    {
        readTimeCounter=0;
        ReadTime();
    }
    if( dslr.enabled != dslr_status)
    {
        dslr_status=dslr.enabled;
        if(dslr.enabled)
        {
//            ui->ledLabelDSLR->setPixmap(ledgreen);
            SetFocusButtonImage(true);
        }
        else
        {
//            ui->ledLabelDSLR->setPixmap(ledred);
            SetFocusButtonImage(false);
        }
    }

    if( (guider.enabled || guider.refreshEnabled) != guider_status )
    {
        guider_status=(guider.enabled || guider.refreshEnabled);
        if(guider.enabled || guider.refreshEnabled)
        {
//            ui->ledLabelGuider->setPixmap(ledgreen);
            SetGuiderButtonImage(true);
        }
        else
        {
//            ui->ledLabelGuider->setPixmap(ledred);
            SetGuiderButtonImage(false);
        }
    }

    if( intervalometersoft.enabled != intervalometersoft_status )
    {
        intervalometersoft_status=intervalometersoft.enabled;
        if(intervalometersoft.enabled)
        {
//            ui->ledLabelSoftIntervalometer->setPixmap(ledgreen);
            SetSoftIntervalometerButtonImage(true);
            intervalometerhard.enabled=false;
            intervalometerhard.UpdateStatus();
            dslr.enableConnect=false;
        }
        else
        {
            intervalometersoft.UpdateStatus();
//            ui->ledLabelSoftIntervalometer->setPixmap(ledred);
            SetSoftIntervalometerButtonImage(false);
            dslr.enableConnect=true;
        }
    }
    if( intervalometerhard.enabled != intervalometerhard_status)
    {
        intervalometerhard_status=intervalometerhard.enabled;
        if(intervalometerhard.enabled)
        {
//            ui->ledLabelHardIntervalometer->setPixmap(ledgreen);
            SetHardIntervalometerButtonImage(true);
            intervalometersoft.enabled=false;
            intervalometersoft.UpdateStatus();
//            ui->ledLabelSoftIntervalometer->setPixmap(ledred);
            SetSoftIntervalometerButtonImage(false);
            dslr.enableConnect=false;
        }
        else
        {
            intervalometerhard.UpdateStatus();
//            ui->ledLabelHardIntervalometer->setPixmap(ledred);
            SetHardIntervalometerButtonImage(false);
            dslr.enableConnect=true;
        }
    }

    if(guider.enabled || guider.refreshEnabled)
    {
        guider.RefreshData();
        if(!guider.interfaceWindowOpen)
        {
            QPixmap guideimage("/run/shm/GuiderWorkingImage.jpg");
            ui->mainImageLabel->setPixmap(guideimage);
            UpdateGuideInfo();
        }
    }

    if( intervalometerhard.enabled)
    {
        intervalometerhard.UpdateStatus();
        UpdateIntervalometerInfo(true);
    }

    if( intervalometersoft.enabled)
    {
        intervalometersoft.UpdateStatus();
        UpdateIntervalometerInfo(false);
    }

    if(dslr.enabled)
        dslr.RefreshData();


}


void PiGuider::on_exitButton_clicked()
{
    close();
}


void PiGuider::on_pushButton_clicked()
{
    lsas.ReadSettings(); // configuration
    lsas.ReadParams(); // parameters
//    lsas.DisplayData();
//    lsas.MakeNegativeParamsZero();
    lsas.DisplayData();
//    lsas.SaveParams();

#ifndef RUNONPC

    if(buttonpressed)
{
    buttonpressed=false;
//    pincontrol.CameraRelease(IVL1);
//    pincontrol.AscensionRelease();
//    usleep(2000000);
//    pincontrol.DeclinationRelease();
}
else
{
    buttonpressed=true;
//    pincontrol.CameraFocus(IVL1);
//    usleep(2000000);
//    pincontrol.CameraShoot(IVL1);

//    pincontrol.AscensionMinus();
//    usleep(2000000);
//    pincontrol.DeclinationPlus();
//    usleep(2000000);
//    pincontrol.AscensionPlus();
//    usleep(2000000);
//    pincontrol.DeclinationMinus();
}


#endif
}




void PiGuider::on_guideButton_clicked()
{
    guider.interfaceWindowOpen=true;
    guider.show();
}

void PiGuider::on_focusButton_clicked()
{
    dslr.show();
//    QPixmap image("media/focus-img.png");
//    ui->mainImageLabel->setPixmap(image);
}



void PiGuider::on_intervalometerSoftButton_clicked()
{
    intervalometersoft.show();
}

void PiGuider::on_intervalometerHardButton_clicked()
{
    intervalometerhard.show();
}

void PiGuider::SetGuiderButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/sections-512x512/Guide-on.png");
    else
        changingButtonsPixmap.load("://media/icons/sections-512x512/Guide.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->guideButton->setIcon(buttonIcon);
    ui->guideButton->setIconSize(buttonSize);
    ui->guideButton->setFixedSize(buttonSize);
}

void PiGuider::SetFocusButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/sections-512x512/ManualFocus-on.png");
    else
        changingButtonsPixmap.load("://media/icons/sections-512x512/ManualFocus.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->focusButton->setIcon(buttonIcon);
    ui->focusButton->setIconSize(buttonSize);
    ui->focusButton->setFixedSize(buttonSize);
}

void PiGuider::SetHardIntervalometerButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer-on.png");
    else
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->intervalometerHardButton->setIcon(buttonIcon);
    ui->intervalometerHardButton->setIconSize(buttonSize);
    ui->intervalometerHardButton->setFixedSize(buttonSize);
}

void PiGuider::SetSoftIntervalometerButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/sections-512x512/SoftIntervalometer-on.png");
    else
        changingButtonsPixmap.load("://media/icons/sections-512x512/SoftIntervalometer.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->intervalometerSoftButton->setIcon(buttonIcon);
    ui->intervalometerSoftButton->setIconSize(buttonSize);
    ui->intervalometerSoftButton->setFixedSize(buttonSize);
}

void PiGuider::NumpadReturnClicked()
{
    switch(numpad.callerName)
    {
    case YEAR:
        ui->yearLineEdit->setText(numpad.inputResult);
        break;
    case MONTH:
        ui->monthLineEdit->setText(numpad.inputResult);
        break;
    case DAY:
        ui->dayLineEdit->setText(numpad.inputResult);
        break;
    case HOUR:
        ui->hoursLineEdit->setText(numpad.inputResult);
        break;
    case MINUTE:
        ui->minutesLineEdit->setText(numpad.inputResult);
        break;
    }
    ReadValues();
    ValidateInput();
    editEnabled=true;
    SetTime();
}

void PiGuider::UpdateGuideInfo()
{
    QString rezMessage;
    if(guider.interfaceWindowOpen)
        return;
    if(guider.ascensionGuiding != raGuideStatus)
    {
        raGuideStatus=guider.ascensionGuiding;
        image.load("://media/icons/tools-32x32/arrowLeftGrey.png");
        ui->leftLabel->setPixmap(image);
        image.load("://media/icons/tools-32x32/arrowRightGrey.png");
        ui->rightLabel->setPixmap(image);

        switch(guider.ascensionGuiding)
        {
        case 1:
            image.load("://media/icons/tools-32x32/arrowRight.png");
            ui->rightLabel->setPixmap(image);
            break;
        case -1:
            image.load("://media/icons/tools-32x32/arrowLeft.png");
            ui->leftLabel->setPixmap(image);
            break;
        }
    }
    if(guider.declinationGuiding != declGuideStatus)
    {
        declGuideStatus=guider.declinationGuiding;
        image.load("://media/icons/tools-32x32/arrowUpGrey.png");
        ui->upLabel->setPixmap(image);
        image.load("://media/icons/tools-32x32/arrowDownGrey.png");
        ui->downLabel->setPixmap(image);

        switch(guider.declinationGuiding)
        {
        case 1:
            image.load("://media/icons/tools-32x32/arrowUp.png");
            ui->upLabel->setPixmap(image);
            break;
        case -1:
            image.load("://media/icons/tools-32x32/arrowDown.png");
            ui->downLabel->setPixmap(image);
            break;
        }
    }

    rezMessage=QString::number((double)guider.raDriftArcsec);
    ui->labelRaDriftValueArcsec->setText(rezMessage);

    rezMessage=QString::number((double)guider.declDriftArcsec);
    ui->labelDeclDriftValueArcsec->setText(rezMessage);

}

void PiGuider::UpdateIntervalometerInfo(bool hardIv)
{
    QString rezMessage;
    if(hardIv)
    {
        if(intervalometerhard.intervalometer1Enabled)
        {
            rezMessage=QString::number((long)intervalometerhard.iterationsCount1);
            ui->iv1CyclesLabel->setText(rezMessage);
            rezMessage=QString::number((long)intervalometerhard.exposurePeriodCount1);
            ui->iv1SecondsLabel->setText(rezMessage);
        }
        else
        {
            ui->iv1CyclesLabel->setText("0");
            ui->iv1SecondsLabel->setText("0");
        }
        if(intervalometerhard.intervalometer2Enabled)
        {
            rezMessage=QString::number((long)intervalometerhard.iterationsCount2);
            ui->iv2CyclesLabel->setText(rezMessage);
            rezMessage=QString::number((long)intervalometerhard.exposurePeriodCount2);
            ui->iv2SecondsLabel->setText(rezMessage);
        }
        else
        {
            ui->iv2CyclesLabel->setText("0");
            ui->iv2SecondsLabel->setText("0");
        }
    }
    else
    {
        ui->iv2CyclesLabel->setText("0");
        ui->iv2SecondsLabel->setText("0");
        if(intervalometersoft.intervalometer1Enabled)
        {
            rezMessage=QString::number((long)intervalometersoft.iterationsCount1);
            ui->iv1CyclesLabel->setText(rezMessage);
            rezMessage=QString::number((long)intervalometersoft.exposurePeriodCount1);
            ui->iv1SecondsLabel->setText(rezMessage);
        }
        else
        {
            ui->iv1CyclesLabel->setText("0");
            ui->iv1SecondsLabel->setText("0");
        }

    }
}

void PiGuider::on_yearLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(YEAR);
    }
}

void PiGuider::on_monthLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(MONTH);
    }
}

void PiGuider::on_dayLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(DAY);
    }
}

void PiGuider::on_hoursLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(HOUR);
    }
}

void PiGuider::on_minutesLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(MINUTE);
    }
}
