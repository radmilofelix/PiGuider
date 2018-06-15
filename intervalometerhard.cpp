#include "intervalometerhard.h"
#include "ui_intervalometerhard.h"
#include <QMessageBox>

///////
#include <iostream>
using namespace std;
/////////

IntervalometerHard::IntervalometerHard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IntervalometerHard)
{
    ui->setupUi(this);
    connect(&numpad, SIGNAL(ReturnClickedSignal()), this, SLOT(NumpadReturnClickedSlot()));
    intervalometer1Enabled=false;
    intervalometer2Enabled=false;
    buttonSize.setHeight(70);
    buttonSize.setWidth(70);
    QPixmap image("media/icons/tools-16x16/led-red.png");
    ui->intervalometer1LedLabel->setPixmap(image);
    ui->intervalometer2LedLabel->setPixmap(image);
    enabled=false;
    editEnabled=true;
    enableVirtualKeyboard=true;
    Init();
    DisplayInputValues();
    HmsToPeriods();
    DisplayCounterValues();
}

IntervalometerHard::~IntervalometerHard()
{
    delete ui;
}

void IntervalometerHard::Init()
{
/*
    iterations1=1;
    iterationsCount1=iterations1;
    jobDelayHours1=0;
    jobDelayHoursCount1=jobDelayHours1;
    jobDelayMinutes1=0;
    jobDelayMinutesCount1=jobDelayMinutes1;
    jobDelaySeconds1=1;
    jobDelaySecondsCount1=jobDelaySeconds1;
    restHours1=0;
    restHoursCount1=restHours1;
    restMinutes1=0;
    restMinutesCount1=restMinutes1;
    restSeconds1=1;
    restSecondsCount1=restSeconds1;
    focusHours1=0;
    focusHoursCount1=focusHours1;
    focusMinutes1=0;
    focusMinutesCount1=focusMinutes1;
    focusSeconds1=1;
    focusSecondsCount1=focusSeconds1;
    exposureHours1=0;
    exposureHoursCount1=exposureHours1;
    exposureMinutes1=0;
    exposureMinutesCount1=exposureMinutes1;
    exposureSeconds1=1;
    exposureSecondsCount1=exposureSeconds1;

    iterations2=1;
    iterationsCount2=iterations2;
    jobDelayHours2=0;
    jobDelayHoursCount2=jobDelayHours2;
    jobDelayMinutes2=0;
    jobDelayMinutesCount2=jobDelayMinutes2;
    jobDelaySeconds2=0;
    jobDelaySecondsCount2=jobDelaySeconds2;
    restHours2=0;
    restHoursCount2=restHours2;
    restMinutes2=0;
    restMinutesCount2=restMinutes2;
    restSeconds2=2;
    restSecondsCount2=restSeconds2;
    focusHours2=0;
    focusHoursCount2=focusHours2;
    focusMinutes2=0;
    focusMinutesCount2=focusMinutes2;
    focusSeconds2=4;
    focusSecondsCount2=focusSeconds2;
    exposureHours2=0;
    exposureHoursCount2=exposureHours2;
    exposureMinutes2=0;
    exposureMinutesCount2=exposureMinutes2;
    exposureSeconds2=5;
    exposureSecondsCount2=exposureSeconds2;
    HmsToPeriods();
*/


    lsas.ReadData("params.cfg" ,"params-default.cfg",false);
    iterations1=lsas.hwIterations1.value;
    iterations2=lsas.hwIterations2.value;

    jobDelayPeriod1=lsas.hwJobDelayPeriod1.value;
    restPeriod1=lsas.hwRestPeriod1.value;
    focusPeriod1=lsas.hwFocusPeriod1.value;
    exposurePeriod1=lsas.hwExposurePeriod1.value;

    jobDelayPeriod2=lsas.hwJobDelayPeriod2.value;
    restPeriod2=lsas.hwRestPeriod2.value;
    focusPeriod2=lsas.hwFocusPeriod2.value;
    exposurePeriod2=lsas.hwExposurePeriod2.value;

    iterationsCount1=iterations1;
    iterationsCount2=iterations2;

    jobDelayPeriodCount1=jobDelayPeriod1;
    restPeriodCount1=restPeriod1;
    focusPeriodCount1=focusPeriod1;
    exposurePeriodCount1=exposurePeriod1;

    jobDelayPeriodCount2=jobDelayPeriod2;
    restPeriodCount2=restPeriod2;
    focusPeriodCount2=focusPeriod2;
    exposurePeriodCount2=exposurePeriod2;

    PeriodsToHMS();



}

void IntervalometerHard::UpdateStatus()
{
    QIcon buttonIcon;
    if(enabled)
    {
        long i=timer.elapsed()/1000-oldTime;
        oldTime=timer.elapsed()/1000;
        if(i>0)
            for(int j=0; j<i; j++)
                DecreaseTimers();
    }
    else
    {
        pincontrol.CameraRelease(1);
        pincontrol.CameraRelease(2);
        ValidateInput();
        HmsToPeriods();
        DisplayCounterValues();
        intervalometer1Enabled=false;
        intervalometer2Enabled=false;
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->intervalometer1Button->setIcon(buttonIcon);
        ui->intervalometer2Button->setIcon(buttonIcon);
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->enableButton->setIcon(buttonIcon);
        ui->enableButton->setIconSize(buttonSize);
        ui->enableButton->setFixedSize(buttonSize);
        ui->intervalometer1Button->setIconSize(buttonSize);
        ui->intervalometer1Button->setFixedSize(buttonSize);
        ui->intervalometer2Button->setIconSize(buttonSize);
        ui->intervalometer2Button->setFixedSize(buttonSize);
    }
}

void IntervalometerHard::ReadValues()
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

    iterations2=ui->iterationsLineEdit2->text().toInt();
    jobDelayHours2=ui->jobDelayHoursLineEdit2->text().toInt();
    jobDelayMinutes2=ui->jobDelayMinutesLineEdit2->text().toInt();
    jobDelaySeconds2=ui->jobDelaySecondsLineEdit2->text().toInt();
    restHours2=ui->restHoursLineEdit2->text().toInt();
    restMinutes2=ui->restMinutesLineEdit2->text().toInt();
    restSeconds2=ui->restSecondsLineEdit2->text().toInt();
    focusHours2=ui->focusHoursLineEdit2->text().toInt();
    focusMinutes2=ui->focusMinutesLineEdit2->text().toInt();
    focusSeconds2=ui->focusSecondsLineEdit2->text().toInt();
    exposureHours2=ui->exposureHoursLineEdit2->text().toInt();
    exposureMinutes2=ui->exposureMinutesLineEdit2->text().toInt();
    exposureSeconds2=ui->exposureSecondsLineEdit2->text().toInt();
}

void IntervalometerHard::ValidateInput()
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

    if(jobDelayMinutes2 < 0)
    {
        jobDelayMinutes2=0;
        ui->jobDelayMinutesLineEdit2->setText("0");
    }
    if(jobDelayMinutes2 > 59)
    {
        jobDelayMinutes2=59;
        ui->jobDelayMinutesLineEdit2->setText("59");
    }
    if(jobDelaySeconds2 < 0)
    {
        jobDelaySeconds2=0;
        ui->jobDelaySecondsLineEdit2->setText("0");
    }
    if(jobDelaySeconds2 > 59)
    {
        jobDelaySeconds2=59;
        ui->jobDelaySecondsLineEdit2->setText("59");
    }
    if(restMinutes2 < 0)
    {
        restMinutes2=0;
        ui->restMinutesLineEdit2->setText("0");
    }
    if(restMinutes2 > 59)
    {
        restMinutes2=59;
        ui->restMinutesLineEdit2->setText("59");
    }
    if(restSeconds2 < 0)
    {
        restSeconds2=0;
        ui->restSecondsLineEdit2->setText("0");
    }
    if(restSeconds2 > 59)
    {
        restSeconds2=59;
        ui->restSecondsLineEdit2->setText("59");
    }
    if(focusMinutes2 < 0)
    {
        focusMinutes2=0;
        ui->focusMinutesLineEdit2->setText("0");
    }
    if(focusMinutes2 > 59)
    {
        focusMinutes2=59;
        ui->focusMinutesLineEdit2->setText("59");
    }
    if(focusSeconds2 < 0)
    {
        focusSeconds2=0;
        ui->focusSecondsLineEdit2->setText("0");
    }
    if(focusSeconds2 > 59)
    {
        focusSeconds2=59;
        ui->focusSecondsLineEdit2->setText("59");
    }
    if(exposureMinutes2 < 0)
    {
        exposureMinutes2=0;
        ui->exposureMinutesLineEdit2->setText("0");
    }
    if(exposureMinutes2 > 59)
    {
        exposureMinutes2=59;
        ui->exposureMinutesLineEdit2->setText("59");
    }
    if(exposureSeconds2 < 0)
    {
        exposureSeconds2=0;
        ui->exposureSecondsLineEdit2->setText("0");
    }
    if(exposureSeconds2 > 59)
    {
        exposureSeconds2=59;
        ui->exposureSecondsLineEdit2->setText("59");
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

    iterationsCount2=iterations2;
    jobDelayHoursCount2=jobDelayHours2;
    jobDelayMinutesCount2=jobDelayMinutes2;
    jobDelaySecondsCount2=jobDelaySeconds2;
    restHoursCount2=restHours2;
    restMinutesCount2=restMinutes2;
    restSecondsCount2=restSeconds2;
    focusHoursCount2=focusHours2;
    focusMinutesCount2=focusMinutes2;
    focusSecondsCount2=focusSeconds2;
    exposureHoursCount2=exposureHours2;
    exposureMinutesCount2=exposureMinutes2;
    exposureSecondsCount2=exposureSeconds2;
    lsas.hwIterations1.value=iterations1;
    lsas.hwIterations2.value=iterations2;
}

void IntervalometerHard::DisplayCounterValues()
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

    ui->iterationsLabel2->setText(QString::number((int)iterationsCount2));
    ui->jobDelayHoursLabel2->setText(QString::number((int)jobDelayHoursCount2));
    ui->jobDelayMinutesLabel2->setText(QString::number((int)jobDelayMinutesCount2));
    ui->jobDelaySecondsLabel2->setText(QString::number((int)jobDelaySecondsCount2));
    ui->restHoursLabel2->setText(QString::number((int)restHoursCount2));
    ui->restMinutesLabel2->setText(QString::number((int)restMinutesCount2));
    ui->restSecondsLabel2->setText(QString::number((int)restSecondsCount2));
    ui->focusHoursLabel2->setText(QString::number((int)focusHoursCount2));
    ui->focusMinutesLabel2->setText(QString::number((int)focusMinutesCount2));
    ui->focusSecondsLabel2->setText(QString::number((int)focusSecondsCount2));
    ui->exposureHoursLabel2->setText(QString::number((int)exposureHoursCount2));
    ui->exposureMinutesLabel2->setText(QString::number((int)exposureMinutesCount2));
    ui->exposureSecondsLabel2->setText(QString::number((int)exposureSecondsCount2));
    ui->labelJobDelay2->setText(QString::number((long)jobDelayPeriodCount2));
    ui->labelRest2->setText(QString::number((long)restPeriodCount2));
    ui->labelFocus2->setText(QString::number((long)focusPeriodCount2));
    ui->labelExposure2->setText(QString::number((long)exposurePeriodCount2));
}

void IntervalometerHard::DisplayInputValues()
{
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

    ui->iterationsLineEdit2->setText(QString::number((int)iterations2));
    ui->jobDelayHoursLineEdit2->setText(QString::number((int)jobDelayHours2));
    ui->jobDelayMinutesLineEdit2->setText(QString::number((int)jobDelayMinutes2));
    ui->jobDelaySecondsLineEdit2->setText(QString::number((int)jobDelaySeconds2));
    ui->restHoursLineEdit2->setText(QString::number((int)restHours2));
    ui->restMinutesLineEdit2->setText(QString::number((int)restMinutes2));
    ui->restSecondsLineEdit2->setText(QString::number((int)restSeconds2));
    ui->focusHoursLineEdit2->setText(QString::number((int)focusHours2));
    ui->focusMinutesLineEdit2->setText(QString::number((int)focusMinutes2));
    ui->focusSecondsLineEdit2->setText(QString::number((int)focusSeconds2));
    ui->exposureHoursLineEdit2->setText(QString::number((int)exposureHours2));
    ui->exposureMinutesLineEdit2->setText(QString::number((int)exposureMinutes2));
    ui->exposureSecondsLineEdit2->setText(QString::number((int)exposureSeconds2));
}


void IntervalometerHard::on_closeButton_clicked()
{
    close();
}

void IntervalometerHard::on_enableButton_clicked()
{
    QIcon buttonIcon;
    if(enabled)
    {
        enabled=false;
        editEnabled=true;
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->enableButton->setIcon(buttonIcon);
    }
    else
    {
        enabled=true;
        editEnabled=false;
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown-on.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->enableButton->setIcon(buttonIcon);
        if(intervalometer1Enabled || intervalometer2Enabled)
        {
            lsas.SaveParams();
            oldTime=0;
            timer.start();
        }
    }
    ui->enableButton->setIconSize(buttonSize);
    ui->enableButton->setFixedSize(buttonSize);
    UpdateStatus();
}

void IntervalometerHard::NumPadCaller(int nameOfControl)
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
        NumpadReturnClickedSlot();
}

long IntervalometerHard::HmsToLong(int hours, int minutes, int seconds)
{
   return (long)hours*3600 + (long)minutes*60 + (long)seconds;

}

void IntervalometerHard::LongToHMS(long HMS, int *hours, int *minutes, int *seconds)
{
    long opModulus = HMS%3600;
    *hours=(HMS-opModulus)/3600;
    *seconds=opModulus%60;
    *minutes=(opModulus-*seconds)/60;
}

void IntervalometerHard::HmsToPeriods()
{
    jobDelayPeriod1 = HmsToLong(jobDelayHours1, jobDelayMinutes1, jobDelaySeconds1);
    restPeriod1 = HmsToLong(restHours1, restMinutes1, restSeconds1);
    focusPeriod1 = HmsToLong(focusHours1, focusMinutes1, focusSeconds1);
    exposurePeriod1 = HmsToLong(exposureHours1, exposureMinutes1, exposureSeconds1);
    jobDelayPeriod2 = HmsToLong(jobDelayHours2, jobDelayMinutes2, jobDelaySeconds2);
    restPeriod2 = HmsToLong(restHours2, restMinutes2, restSeconds2);
    focusPeriod2= HmsToLong(focusHours2, focusMinutes2, focusSeconds2);
    exposurePeriod2 = HmsToLong(exposureHours2, exposureMinutes2, exposureSeconds2);

    jobDelayPeriodCount1 = HmsToLong(jobDelayHoursCount1, jobDelayMinutesCount1, jobDelaySecondsCount1);
    restPeriodCount1 = HmsToLong(restHoursCount1, restMinutesCount1, restSecondsCount1);
    focusPeriodCount1 = HmsToLong(focusHoursCount1, focusMinutesCount1, focusSecondsCount1);
    exposurePeriodCount1 = HmsToLong(exposureHoursCount1, exposureMinutesCount1, exposureSecondsCount1);
    jobDelayPeriodCount2 = HmsToLong(jobDelayHoursCount2, jobDelayMinutesCount2, jobDelaySecondsCount2);
    restPeriodCount2 = HmsToLong(restHoursCount2, restMinutesCount2, restSecondsCount2);
    focusPeriodCount2= HmsToLong(focusHoursCount2, focusMinutesCount2, focusSecondsCount2);
    exposurePeriodCount2 = HmsToLong(exposureHoursCount2, exposureMinutesCount2, exposureSecondsCount2);

    lsas.hwJobDelayPeriod1.value=jobDelayPeriod1;
    lsas.hwRestPeriod1.value=restPeriod1;
    lsas.hwFocusPeriod1.value=focusPeriod1;
    lsas.hwExposurePeriod1.value=exposurePeriod1;

    lsas.hwJobDelayPeriod2.value=jobDelayPeriod2;
    lsas.hwRestPeriod2.value=restPeriod2;
    lsas.hwFocusPeriod2.value=focusPeriod2;
    lsas.hwExposurePeriod2.value=exposurePeriod2;

}

void IntervalometerHard::PeriodsToHMS()
{
    LongToHMS(jobDelayPeriod1, &jobDelayHours1, &jobDelayMinutes1, &jobDelaySeconds1);
    LongToHMS(restPeriod1, &restHours1, &restMinutes1, &restSeconds1);
    LongToHMS(focusPeriod1, &focusHours1, &focusMinutes1, &focusSeconds1);
    LongToHMS(exposurePeriod1, &exposureHours1, &exposureMinutes1, &exposureSeconds1);
    LongToHMS(jobDelayPeriod2, &jobDelayHours2, &jobDelayMinutes2, &jobDelaySeconds2);
    LongToHMS(restPeriod2, &restHours2, &restMinutes2, &restSeconds2);
    LongToHMS(focusPeriod2, &focusHours2, &focusMinutes2, &focusSeconds2);
    LongToHMS(exposurePeriod2, &exposureHours2, &exposureMinutes2, &exposureSeconds2);

    LongToHMS(jobDelayPeriodCount1, &jobDelayHoursCount1, &jobDelayMinutesCount1, &jobDelaySecondsCount1);
    LongToHMS(restPeriodCount1, &restHoursCount1, &restMinutesCount1, &restSecondsCount1);
    LongToHMS(focusPeriodCount1, &focusHoursCount1, &focusMinutesCount1, &focusSecondsCount1);
    LongToHMS(exposurePeriodCount1, &exposureHoursCount1, &exposureMinutesCount1, &exposureSecondsCount1);
    LongToHMS(jobDelayPeriodCount2, &jobDelayHoursCount2, &jobDelayMinutesCount2, &jobDelaySecondsCount2);
    LongToHMS(restPeriodCount2, &restHoursCount2, &restMinutesCount2, &restSecondsCount2);
    LongToHMS(focusPeriodCount2, &focusHoursCount2, &focusMinutesCount2, &focusSecondsCount2);
    LongToHMS(exposurePeriodCount2, &exposureHoursCount2, &exposureMinutesCount2, &exposureSecondsCount2);
}

void IntervalometerHard::DecreaseTimer1()
{
    QIcon buttonIcon;
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
                pincontrol.CameraFocus(1);
                focusPeriodCount1--;
                return;
            }
            if(exposurePeriodCount1)
            {
                pincontrol.CameraShoot(1);
                exposurePeriodCount1--;
                return;
            }
            else
            {
                pincontrol.CameraRelease(1);
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

            changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer.png");
            buttonIcon.addPixmap(changingButtonsPixmap);
            ui->intervalometer1Button->setIcon(buttonIcon);
            ui->intervalometer1Button->setIconSize(buttonSize);
            ui->intervalometer1Button->setFixedSize(buttonSize);
        }
    }
}

void IntervalometerHard::DecreaseTimer2()
{
    QIcon buttonIcon;
    if(intervalometer2Enabled)
    {
        if(jobDelayPeriodCount2>0)
        {
            jobDelayPeriodCount2--;
            return;
        }
        if(iterationsCount2)
        {
            if(restPeriodCount2)
            {
                restPeriodCount2--;
                return;
            }
            if(focusPeriodCount2)
            {
                pincontrol.CameraFocus(2);
                focusPeriodCount2--;
                return;
            }
            if(exposurePeriodCount2)
            {
                pincontrol.CameraShoot(2);
                exposurePeriodCount2--;
                return;
            }
            else
            {
                pincontrol.CameraRelease(2);
                iterationsCount2--;
                restPeriodCount2=restPeriod2;
                focusPeriodCount2=focusPeriod2;
                exposurePeriodCount2=exposurePeriod2;
            }
        }
        else
        {
            iterationsCount2=iterations2;
            jobDelayPeriodCount2=jobDelayPeriod2;
            restPeriodCount2=restPeriod2;
            focusPeriodCount2=focusPeriod2;
            exposurePeriodCount2=exposurePeriod2;
            intervalometer2Enabled=false;

            changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer.png");
            buttonIcon.addPixmap(changingButtonsPixmap);
            ui->intervalometer2Button->setIcon(buttonIcon);
            ui->intervalometer2Button->setIconSize(buttonSize);
            ui->intervalometer2Button->setFixedSize(buttonSize);
        }
    }
}

void IntervalometerHard::DecreaseTimers()
{
    QIcon buttonIcon;
    DecreaseTimer1();
    DecreaseTimer2();
    if( !intervalometer1Enabled && !intervalometer2Enabled)
    {
        enabled=false;
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->enableButton->setIcon(buttonIcon);
        ui->enableButton->setIconSize(buttonSize);
        ui->enableButton->setFixedSize(buttonSize);    }
    PeriodsToHMS();
    DisplayCounterValues();
}

void IntervalometerHard::NumpadReturnClickedSlot()
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

    case ITERATIONS2:
        ui->iterationsLineEdit2->setText(numpad.inputResult);
        break;
    case JOBDELAYHOURS2:
        ui->jobDelayHoursLineEdit2->setText(numpad.inputResult);
        break;
    case JOBDELAYMINUTES2:
        ui->jobDelayMinutesLineEdit2->setText(numpad.inputResult);
        break;
    case JOBDELAYSECONDS2:
        ui->jobDelaySecondsLineEdit2->setText(numpad.inputResult);
        break;
    case RESTHOURS2:
        ui->restHoursLineEdit2->setText(numpad.inputResult);
        break;
    case RESTMINUTES2:
        ui->restMinutesLineEdit2->setText(numpad.inputResult);
        break;
    case RESTSECONDS2:
        ui->restSecondsLineEdit2->setText(numpad.inputResult);
        break;
    case FOCUSHOURS2:
        ui->focusHoursLineEdit2->setText(numpad.inputResult);
        break;
    case FOCUSMINUTES2:
        ui->focusMinutesLineEdit2->setText(numpad.inputResult);
        break;
    case FOCUSSECONDS2:
        ui->focusSecondsLineEdit2->setText(numpad.inputResult);
        break;
    case EXPOSUREHOURS2:
        ui->exposureHoursLineEdit2->setText(numpad.inputResult);
        break;
    case EXPOSUREMINUTES2:
        ui->exposureMinutesLineEdit2->setText(numpad.inputResult);
        break;
    case EXPOSURESECONDS2:
        ui->exposureSecondsLineEdit2->setText(numpad.inputResult);
        break;
    }
    ReadValues();
    ValidateInput();
    HmsToPeriods();
    DisplayCounterValues();
    editEnabled=true;
}


void IntervalometerHard::on_pushButton_clicked()
{
    DecreaseTimers();
//    PeriodsToHMS();
//    DisplayCounterValues();
}

void IntervalometerHard::on_intervalometer1Button_clicked()
{
    QIcon buttonIcon;
    QPixmap image("media/icons/tools-16x16/led-red.png");
    if(intervalometer1Enabled)
    {
        intervalometer1Enabled=false;
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->intervalometer1Button->setIcon(buttonIcon);
    }
    else
    {
        image.load("media/icons/tools-16x16/led-green.png");
        intervalometer1Enabled=true;
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer-on.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->intervalometer1Button->setIcon(buttonIcon);
    }
    ui->intervalometer1LedLabel->setPixmap(image);
    ui->intervalometer1Button->setIconSize(buttonSize);
    ui->intervalometer1Button->setFixedSize(buttonSize);
}

void IntervalometerHard::on_intervalometer2Button_clicked()
{
    QIcon buttonIcon;
    QPixmap image("media/icons/tools-16x16/led-red.png");
    if(intervalometer2Enabled)
    {
        intervalometer2Enabled=false;
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->intervalometer2Button->setIcon(buttonIcon);
    }
    else
    {
        image.load("media/icons/tools-16x16/led-green.png");
        intervalometer2Enabled=true;
        changingButtonsPixmap.load("://media/icons/sections-512x512/HardIntervalometer-on.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->intervalometer2Button->setIcon(buttonIcon);
    }
    ui->intervalometer2LedLabel->setPixmap(image);
    ui->intervalometer2Button->setIconSize(buttonSize);
    ui->intervalometer2Button->setFixedSize(buttonSize);
}

void IntervalometerHard::on_exposureHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSUREHOURS1);
    }
}

void IntervalometerHard::on_exposureMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSUREMINUTES1);
    }
}

void IntervalometerHard::on_exposureSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSURESECONDS1);
    }
}

void IntervalometerHard::on_focusHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSHOURS1);
    }
    else
        ui->focusHoursLineEdit1->disconnect();
}

void IntervalometerHard::on_focusMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSMINUTES1);
    }
}

void IntervalometerHard::on_focusSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSSECONDS1);
    }
}

void IntervalometerHard::on_restHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTHOURS1);
    }
}

void IntervalometerHard::on_restMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTMINUTES1);
    }
}

void IntervalometerHard::on_restSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTSECONDS1);
    }
}

void IntervalometerHard::on_jobDelayHoursLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYHOURS1);
    }
}

void IntervalometerHard::on_jobDelayMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYMINUTES1);
    }
}

void IntervalometerHard::on_jobDelaySecondsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYSECONDS1);
    }
}





void IntervalometerHard::on_exposureHoursLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSUREHOURS2);
    }
}

void IntervalometerHard::on_exposureMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSUREMINUTES2);
    }
}

void IntervalometerHard::on_exposureSecondsLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(EXPOSURESECONDS2);
    }
}

void IntervalometerHard::on_focusHoursLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSHOURS2);
    }
}

void IntervalometerHard::on_focusMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSMINUTES2);
    }
}

void IntervalometerHard::on_focusSecondsLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(FOCUSSECONDS2);
    }
}

void IntervalometerHard::on_restHoursLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTHOURS2);
    }
}

void IntervalometerHard::on_restMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTMINUTES2);
    }
}

void IntervalometerHard::on_restSecondsLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(RESTSECONDS2);
    }
}

void IntervalometerHard::on_jobDelayHoursLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYHOURS2);
    }
}

void IntervalometerHard::on_jobDelayMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYMINUTES2);
    }
}

void IntervalometerHard::on_jobDelaySecondsLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(JOBDELAYSECONDS2);
    }
}

void IntervalometerHard::on_iterationsLineEdit1_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(ITERATIONS1);
    }
}

void IntervalometerHard::on_iterationsLineEdit2_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller(ITERATIONS2);
    }
}

void IntervalometerHard::on_toggleVirtualNumpadButton_clicked()
{
    QIcon buttonIcon;
    if(enableVirtualKeyboard)
    {
        changingButtonsPixmap.load("://media/icons/tools-512x512/numpad.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->toggleVirtualNumpadButton->setIcon(buttonIcon);
        enableVirtualKeyboard=false;
    }
    else
    {
        changingButtonsPixmap.load("://media/icons/tools-512x512/numpad-on.png");
        buttonIcon.addPixmap(changingButtonsPixmap);
        ui->toggleVirtualNumpadButton->setIcon(buttonIcon);
        enableVirtualKeyboard=true;
    }
    ui->toggleVirtualNumpadButton->setIconSize(buttonSize);
    ui->toggleVirtualNumpadButton->setFixedSize(buttonSize);
}
