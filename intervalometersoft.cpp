#include "intervalometersoft.h"
#include "ui_intervalometersoft.h"


#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>

using namespace cv;

IntervalometerSoft::IntervalometerSoft(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IntervalometerSoft)
{
    ui->setupUi(this);
    connect(&numpad, SIGNAL(ReturnClickedSignal()), this, SLOT(NumpadReturnClicked()));
    enabled=false;
    buttonSize.setHeight(70);
    buttonSize.setWidth(70);
    intervalometer1Enabled=false;
    enabled=false;
    editEnabled=true;
    enableVirtualKeyboard=true;
    Init();
    DisplayInputValues();
    HmsToPeriods();
    DisplayCounterValues();
}

IntervalometerSoft::~IntervalometerSoft()
{
    delete ui;
}

void IntervalometerSoft::Init()
{
    lsas.ReadData("params.cfg" ,"params-default.cfg",false);
    iterations1=lsas.swIterations1.value;

    jobDelayPeriod1=lsas.swJobDelayPeriod1.value;
    restPeriod1=lsas.swRestPeriod1.value;
    focusPeriod1=lsas.swFocusPeriod1.value;
    exposurePeriod1=lsas.swExposurePeriod1.value;

    iterationsCount1=iterations1;

    jobDelayPeriodCount1=jobDelayPeriod1;
    restPeriodCount1=restPeriod1;
    focusPeriodCount1=focusPeriod1;
    exposurePeriodCount1=exposurePeriod1;

    PeriodsToHMS();
}


void IntervalometerSoft::UpdateStatus()
{
    if(enabled)
    {
        long i=timer.elapsed()/1000-oldTime;
        oldTime=timer.elapsed()/1000;
        if(i>0)
            for(int j=0; j<i; j++)
                DecreaseTimers();
//        SetEnableButtonImage(true);
    }
    else
    {
//        pincontrol.CameraRelease(1);
//        pincontrol.CameraRelease(2);
        ValidateInput();
        HmsToPeriods();
        DisplayCounterValues();
        intervalometer1Enabled=false;
        editEnabled=true;
        SetEnableButtonImage(false);
    }
}

void IntervalometerSoft::ReadValues()
{
    iterations1=ui->iterationsLineEdit1->text().toInt();
    jobDelayHours1=ui->jobDelayHoursLineEdit1->text().toInt();
    jobDelayMinutes1=ui->jobDelayMinutesLineEdit1->text().toInt();
    jobDelaySeconds1=ui->jobDelaySecondsLineEdit1->text().toInt();
    restHours1=ui->restHoursLineEdit1->text().toInt();
    restMinutes1=ui->restMinutesLineEdit1->text().toInt();
    restSeconds1=ui->restSecondsLineEdit1->text().toInt();
    focusHours1=ui->focusHoursLineEdit1->text().toInt();
    focusMinutes1=ui->focusMinutesLineEdit1->text().toInt();
    focusSeconds1=ui->focusSecondsLineEdit1->text().toInt();
    exposureHours1=ui->exposureHoursLineEdit1->text().toInt();
    exposureMinutes1=ui->exposureMinutesLineEdit1->text().toInt();
    exposureSeconds1=ui->exposureSecondsLineEdit1->text().toInt();
}

void IntervalometerSoft::ValidateInput()
{
    if(jobDelayMinutes1 < 0)
    {
        jobDelayMinutes1=0;
        ui->jobDelayMinutesLineEdit1->setText("0");
    }
    if(jobDelayMinutes1 > 59)
    {
        jobDelayMinutes1=59;
        ui->jobDelayMinutesLineEdit1->setText("59");
    }
    if(jobDelaySeconds1 < 0)
    {
        jobDelaySeconds1=0;
        ui->jobDelaySecondsLineEdit1->setText("0");
    }
    if(jobDelaySeconds1 > 59)
    {
        jobDelaySeconds1=59;
        ui->jobDelaySecondsLineEdit1->setText("59");
    }
    if(restMinutes1 < 0)
    {
        restMinutes1=0;
        ui->restMinutesLineEdit1->setText("0");
    }
    if(restMinutes1 > 59)
    {
        restMinutes1=59;
        ui->restMinutesLineEdit1->setText("59");
    }
    if(restSeconds1 < 0)
    {
        restSeconds1=0;
        ui->restSecondsLineEdit1->setText("0");
    }
    if(restSeconds1 > 59)
    {
        restSeconds1=59;
        ui->restSecondsLineEdit1->setText("59");
    }
    if(focusMinutes1 < 0)
    {
        focusMinutes1=0;
        ui->focusMinutesLineEdit1->setText("0");
    }
    if(focusMinutes1 > 59)
    {
        focusMinutes1=59;
        ui->focusMinutesLineEdit1->setText("59");
    }
    if(focusSeconds1 < 0)
    {
        focusSeconds1=0;
        ui->focusSecondsLineEdit1->setText("0");
    }
    if(focusSeconds1 > 59)
    {
        focusSeconds1=59;
        ui->focusSecondsLineEdit1->setText("59");
    }
    if(exposureMinutes1 < 0)
    {
        exposureMinutes1=0;
        ui->exposureMinutesLineEdit1->setText("0");
    }
    if(exposureMinutes1 > 59)
    {
        exposureMinutes1=59;
        ui->exposureMinutesLineEdit1->setText("59");
    }
    if(exposureSeconds1 < 0)
    {
        exposureSeconds1=0;
        ui->exposureSecondsLineEdit1->setText("0");
    }
    if(exposureSeconds1 > 59)
    {
        exposureSeconds1=59;
        ui->exposureSecondsLineEdit1->setText("59");
    }

    iterationsCount1=iterations1;
    jobDelayHoursCount1=jobDelayHours1;
    jobDelayMinutesCount1=jobDelayMinutes1;
    jobDelaySecondsCount1=jobDelaySeconds1;
    restHoursCount1=restHours1;
    restMinutesCount1=restMinutes1;
    restSecondsCount1=restSeconds1;
    focusHoursCount1=focusHours1;
    focusMinutesCount1=focusMinutes1;
    focusSecondsCount1=focusSeconds1;
    exposureHoursCount1=exposureHours1;
    exposureMinutesCount1=exposureMinutes1;
    exposureSecondsCount1=exposureSeconds1;

    lsas.swIterations1.value=iterations1;
}

void IntervalometerSoft::DisplayCounterValues()
{
    ui->iterationsLabel1->setText(QString::number((int)iterationsCount1));
    ui->jobDelayHoursLabel1->setText(QString::number((int)jobDelayHoursCount1));
    ui->jobDelayMinutesLabel1->setText(QString::number((int)jobDelayMinutesCount1));
    ui->jobDelaySecondsLabel1->setText(QString::number((int)jobDelaySecondsCount1));
    ui->restHoursLabel1->setText(QString::number((int)restHoursCount1));
    ui->restMinutesLabel1->setText(QString::number((int)restMinutesCount1));
    ui->restSecondsLabel1->setText(QString::number((int)restSecondsCount1));
    ui->focusHoursLabel1->setText(QString::number((int)focusHoursCount1));
    ui->focusMinutesLabel1->setText(QString::number((int)focusMinutesCount1));
    ui->focusSecondsLabel1->setText(QString::number((int)focusSecondsCount1));
    ui->exposureHoursLabel1->setText(QString::number((int)exposureHoursCount1));
    ui->exposureMinutesLabel1->setText(QString::number((int)exposureMinutesCount1));
    ui->exposureSecondsLabel1->setText(QString::number((int)exposureSecondsCount1));
    ui->labelJobDelay1->setText(QString::number((long)jobDelayPeriodCount1));
    ui->labelRest1->setText(QString::number((long)restPeriodCount1));
    ui->labelFocus1->setText(QString::number((long)focusPeriodCount1));
    ui->labelExposure1->setText(QString::number((long)exposurePeriodCount1));
}

void IntervalometerSoft::DisplayInputValues()
{
    editEnabled=false;
    ui->iterationsLineEdit1->setText(QString::number((int)iterations1));
    ui->jobDelayHoursLineEdit1->setText(QString::number((int)jobDelayHours1));
    ui->jobDelayMinutesLineEdit1->setText(QString::number((int)jobDelayMinutes1));
    ui->jobDelaySecondsLineEdit1->setText(QString::number((int)jobDelaySeconds1));
    ui->restHoursLineEdit1->setText(QString::number((int)restHours1));
    ui->restMinutesLineEdit1->setText(QString::number((int)restMinutes1));
    ui->restSecondsLineEdit1->setText(QString::number((int)restSeconds1));
    ui->focusHoursLineEdit1->setText(QString::number((int)focusHours1));
    ui->focusMinutesLineEdit1->setText(QString::number((int)focusMinutes1));
    ui->focusSecondsLineEdit1->setText(QString::number((int)focusSeconds1));
    ui->exposureHoursLineEdit1->setText(QString::number((int)exposureHours1));
    ui->exposureMinutesLineEdit1->setText(QString::number((int)exposureMinutes1));
    ui->exposureSecondsLineEdit1->setText(QString::number((int)exposureSeconds1));
    editEnabled=true;
}


void IntervalometerSoft::NumPadCaller(int nameOfControl)
{
    if(!numpad.busy && enableVirtualKeyboard)
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

long IntervalometerSoft::HmsToLong(int hours, int minutes, int seconds)
{
   return (long)hours*3600 + (long)minutes*60 + (long)seconds;

}

void IntervalometerSoft::LongToHMS(long HMS, int *hours, int *minutes, int *seconds)
{
    long opModulus = HMS%3600;
    *hours=(HMS-opModulus)/3600;
    *seconds=opModulus%60;
    *minutes=(opModulus-*seconds)/60;
}

void IntervalometerSoft::HmsToPeriods()
{
    jobDelayPeriod1 = HmsToLong(jobDelayHours1, jobDelayMinutes1, jobDelaySeconds1);
    restPeriod1 = HmsToLong(restHours1, restMinutes1, restSeconds1);
    focusPeriod1 = HmsToLong(focusHours1, focusMinutes1, focusSeconds1);
    exposurePeriod1 = HmsToLong(exposureHours1, exposureMinutes1, exposureSeconds1);

    jobDelayPeriodCount1 = HmsToLong(jobDelayHoursCount1, jobDelayMinutesCount1, jobDelaySecondsCount1);
    restPeriodCount1 = HmsToLong(restHoursCount1, restMinutesCount1, restSecondsCount1);
    focusPeriodCount1 = HmsToLong(focusHoursCount1, focusMinutesCount1, focusSecondsCount1);
    exposurePeriodCount1 = HmsToLong(exposureHoursCount1, exposureMinutesCount1, exposureSecondsCount1);

    lsas.swJobDelayPeriod1.value=jobDelayPeriod1;
    lsas.swRestPeriod1.value=restPeriod1;
    lsas.swFocusPeriod1.value=focusPeriod1;
    lsas.swExposurePeriod1.value=exposurePeriod1;
}

void IntervalometerSoft::PeriodsToHMS()
{
    LongToHMS(jobDelayPeriod1, &jobDelayHours1, &jobDelayMinutes1, &jobDelaySeconds1);
    LongToHMS(restPeriod1, &restHours1, &restMinutes1, &restSeconds1);
    LongToHMS(focusPeriod1, &focusHours1, &focusMinutes1, &focusSeconds1);
    LongToHMS(exposurePeriod1, &exposureHours1, &exposureMinutes1, &exposureSeconds1);

    LongToHMS(jobDelayPeriodCount1, &jobDelayHoursCount1, &jobDelayMinutesCount1, &jobDelaySecondsCount1);
    LongToHMS(restPeriodCount1, &restHoursCount1, &restMinutesCount1, &restSecondsCount1);
    LongToHMS(focusPeriodCount1, &focusHoursCount1, &focusMinutesCount1, &focusSecondsCount1);
    LongToHMS(exposurePeriodCount1, &exposureHoursCount1, &exposureMinutesCount1, &exposureSecondsCount1);
}

void IntervalometerSoft::DecreaseTimer1()
{
    if(intervalometer1Enabled)
    {
        if(jobDelayPeriodCount1>0)
        {
            jobDelayPeriodCount1--;
            return;
        }
        if(iterationsCount1)
        {
            if(restPeriodCount1)
            {
                restPeriodCount1--;
                return;
            }
            if(focusPeriodCount1)
            {
//                pincontrol.CameraFocus(1);
                focusPeriodCount1--;
                return;
            }
            if(exposurePeriodCount1)
            {
//                pincontrol.CameraShoot(1);
                dslrCamera.ShootOn();
                exposurePeriodCount1--;
                return;
            }
            else
            {

//                pincontrol.CameraRelease(1);
                dslrCamera.ShootRelease();
                iterationsCount1--;
                restPeriodCount1=restPeriod1;
                focusPeriodCount1=focusPeriod1;
                exposurePeriodCount1=exposurePeriod1;
            }
        }
        else
        {
            iterationsCount1=iterations1;
            jobDelayPeriodCount1=jobDelayPeriod1;
            restPeriodCount1=restPeriod1;
            focusPeriodCount1=focusPeriod1;
            exposurePeriodCount1=exposurePeriod1;
            intervalometer1Enabled=false;
        }
    }
}


void IntervalometerSoft::DecreaseTimers()
{
    DecreaseTimer1();
    if( !intervalometer1Enabled)
    {
        enabled=false;

        SetEnableButtonImage(false);
    }
    PeriodsToHMS();
    DisplayCounterValues();
}

void IntervalometerSoft::FrameMessage(QString message)
{
    message=" "+message+" ";
    ui->labelMessages->setText(message);
    ui->labelMessages->adjustSize();
}

void IntervalometerSoft::NumpadReturnClicked()
{
    switch(numpad.callerName)
    {
    case ITERATIONS1:
        ui->iterationsLineEdit1->setText(numpad.inputResult);
        break;
    case JOBDELAYHOURS1:
        ui->jobDelayHoursLineEdit1->setText(numpad.inputResult);
        break;
    case JOBDELAYMINUTES1:
        ui->jobDelayMinutesLineEdit1->setText(numpad.inputResult);
        break;
    case JOBDELAYSECONDS1:
        ui->jobDelaySecondsLineEdit1->setText(numpad.inputResult);
        break;
    case RESTHOURS1:
        ui->restHoursLineEdit1->setText(numpad.inputResult);
        break;
    case RESTMINUTES1:
        ui->restMinutesLineEdit1->setText(numpad.inputResult);
        break;
    case RESTSECONDS1:
        ui->restSecondsLineEdit1->setText(numpad.inputResult);
        break;
    case FOCUSHOURS1:
        ui->focusHoursLineEdit1->setText(numpad.inputResult);
        break;
    case FOCUSMINUTES1:
        ui->focusMinutesLineEdit1->setText(numpad.inputResult);
        break;
    case FOCUSSECONDS1:
        ui->focusSecondsLineEdit1->setText(numpad.inputResult);
        break;
    case EXPOSUREHOURS1:
        ui->exposureHoursLineEdit1->setText(numpad.inputResult);
        break;
    case EXPOSUREMINUTES1:
        ui->exposureMinutesLineEdit1->setText(numpad.inputResult);
        break;
    case EXPOSURESECONDS1:
        ui->exposureSecondsLineEdit1->setText(numpad.inputResult);
        break;
    }
    ReadValues();
    ValidateInput();
    HmsToPeriods();
    DisplayCounterValues();
    editEnabled=true;
}


void IntervalometerSoft::SetEnableButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->enableButton->setIcon(buttonIcon);
    ui->enableButton->setIconSize(buttonSize);
    ui->enableButton->setFixedSize(buttonSize);
}

void IntervalometerSoft::SetConnectButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/connect-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/connect.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->connectButton->setIcon(buttonIcon);
    ui->connectButton->setIconSize(buttonSize);
    ui->connectButton->setFixedSize(buttonSize);
}

void IntervalometerSoft::SetToggleNumpadButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/numpad-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/numpad.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->toggleVirtualNumpadButton->setIcon(buttonIcon);
    ui->toggleVirtualNumpadButton->setIconSize(buttonSize);
    ui->toggleVirtualNumpadButton->setFixedSize(buttonSize);
}


void IntervalometerSoft::on_closeButton_clicked()
{
    close();
}

void IntervalometerSoft::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        editEnabled=true;
        intervalometer1Enabled=false;
        SetEnableButtonImage(false);
    }
    else
    {
        if(dslrCamera.isCamera && dslrCamera.isCameraFile)
        {
            enabled=true;
            editEnabled=false;
            intervalometer1Enabled=true;
            SetEnableButtonImage(true);
            lsas.SaveParams();
            oldTime=0;
            timer.start();
        }
        else
            FrameMessage("DSLR Camera not connected!");
    }
    UpdateStatus();
}




void IntervalometerSoft::on_toggleVirtualNumpadButton_clicked()
{
    if(enableVirtualKeyboard)
    {
        SetToggleNumpadButtonImage(false);
        enableVirtualKeyboard=false;
    }
    else
    {
        SetToggleNumpadButtonImage(true);
        enableVirtualKeyboard=true;
    }
}

void IntervalometerSoft::on_pushButton_clicked()
{
    intervalometer1Enabled=true;
    DecreaseTimers();
//    PeriodsToHMS();
//    DisplayCounterValues();
}


void IntervalometerSoft::on_exposureHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSUREHOURS1);
    }
}

void IntervalometerSoft::on_exposureMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSUREMINUTES1);
    }
}

void IntervalometerSoft::on_exposureSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSURESECONDS1);
    }
}

void IntervalometerSoft::on_focusHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSHOURS1);
    }
    else
        ui->focusHoursLineEdit1->disconnect();
}

void IntervalometerSoft::on_focusMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSMINUTES1);
    }
}

void IntervalometerSoft::on_focusSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)if(dslrCamera.isCamera && dslrCamera.isCameraFile)
    {
        editEnabled=false;
        NumPadCaller(FOCUSSECONDS1);
    }
}

void IntervalometerSoft::on_restHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTHOURS1);
    }
}

void IntervalometerSoft::on_restMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTMINUTES1);
    }
}

void IntervalometerSoft::on_restSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTSECONDS1);
    }
}

void IntervalometerSoft::on_jobDelayHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYHOURS1);
    }
}

void IntervalometerSoft::on_jobDelayMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYMINUTES1);
    }
}

void IntervalometerSoft::on_jobDelaySecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYSECONDS1);
    }
}



void IntervalometerSoft::on_iterationsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(ITERATIONS1);
    }
}

void IntervalometerSoft::on_connectButton_clicked()
{
    if(dslrCamera.isCamera && dslrCamera.isCameraFile)
    {
        if(!dslrCamera.Disconnect())
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }
        FrameMessage((dslrCamera.dslrMessage));
        intervalometer1Enabled=false;
        SetConnectButtonImage(false);
    }
    else
    {
        if(!dslrCamera.Connect())
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }
        FrameMessage((dslrCamera.dslrMessage));
        intervalometer1Enabled=true;
        dslrCamera.SetCameraLastFileNumber();
        SetConnectButtonImage(true);
    }
}
