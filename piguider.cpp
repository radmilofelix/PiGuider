#include "piguider.h"
#include "ui_piguider.h"
#include <QTimer>
//#include <QTime>


#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "globalsettings.h"



//using namespace cv;


PiGuider::PiGuider(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PiGuider)
{
    //  Remove main window frame
    this->setWindowFlags( Qt::FramelessWindowHint );
    ui->setupUi(this);
    dslr_status=false;
    guider_status=false;
    intervalometersoft_status=false;
    intervalometerhard_status=false;
    buttonSize.setHeight(70);
    buttonSize.setWidth(70);

    QPixmap image("://media/icons/tools-16x16/led-red.png");
    ui->ledLabelGuider->setPixmap(image);
    ui->ledLabelDSLR->setPixmap(image);
    ui->ledLabelSoftIntervalometer->setPixmap(image);
    ui->ledLabelHardIntervalometer->setPixmap(image);

    timerMain = new QTimer(this);

    buttonpressed=false;
    QObject::connect(timerMain, SIGNAL(timeout()), this, SLOT(UpdateTime()));
//    timerMain->start(1000);
    timerMain->start(500);
}

PiGuider::~PiGuider()
{
    delete ui;
    delete timerMain;
}

void PiGuider::UpdateTime()
{
    UpdateStatus();
}

void PiGuider::UpdateStatus()
{
    QPixmap ledred("://media/icons/tools-16x16/led-red.png");
    QPixmap ledgreen("://media/icons/tools-16x16/led-green.png");

    if( dslr.enabled != dslr_status)
    {
        dslr_status=dslr.enabled;
        if(dslr.enabled)
        {
            ui->ledLabelDSLR->setPixmap(ledgreen);
            SetFocusButtonImage(true);
        }
        else
        {
            ui->ledLabelDSLR->setPixmap(ledred);
            SetFocusButtonImage(false);
        }
    }

    if( (guider.enabled || guider.refreshEnabled) != guider_status )
    {
        guider_status=(guider.enabled || guider.refreshEnabled);
        if(guider.enabled || guider.refreshEnabled)
        {
            ui->ledLabelGuider->setPixmap(ledgreen);
            SetGuiderButtonImage(true);
        }
        else
        {
            ui->ledLabelGuider->setPixmap(ledred);
            SetGuiderButtonImage(false);
        }
    }

    if( intervalometersoft.enabled != intervalometersoft_status )
    {
        intervalometersoft_status=intervalometersoft.enabled;
        if(intervalometersoft.enabled)
        {
            ui->ledLabelSoftIntervalometer->setPixmap(ledgreen);
            SetSoftIntervlometerButtonImage(true);
            intervalometerhard.enabled=false;
            intervalometerhard.UpdateStatus();
        }
        else
        {
            intervalometersoft.UpdateStatus();
            ui->ledLabelSoftIntervalometer->setPixmap(ledred);
            SetSoftIntervlometerButtonImage(false);
        }
    }
    if( intervalometerhard.enabled != intervalometerhard_status)
    {
        intervalometerhard_status=intervalometerhard.enabled;
        if(intervalometerhard.enabled)
        {
            ui->ledLabelHardIntervalometer->setPixmap(ledgreen);
            SetHardIntervlometerButtonImage(true);
            intervalometersoft.enabled=false;
            intervalometersoft.UpdateStatus();
            ui->ledLabelSoftIntervalometer->setPixmap(ledred);
            SetSoftIntervlometerButtonImage(false);
        }
        else
        {
            intervalometerhard.UpdateStatus();
            ui->ledLabelHardIntervalometer->setPixmap(ledred);
            SetHardIntervlometerButtonImage(false);
        }
    }

    if(guider.enabled || guider.refreshEnabled)
    {
        guider.RefreshData();
        if(!guider.interfaceWindowOpen)
        {
            QPixmap guideimage("/run/shm/GuiderWorkingImage.jpg");
            ui->mainImageLabel->setPixmap(guideimage);
        }
    }

    if( intervalometerhard.enabled)
        intervalometerhard.UpdateStatus();

    if( intervalometersoft.enabled)
        intervalometersoft.UpdateStatus();

    if(dslr.enabled)
        dslr.RefreshData();


}


void PiGuider::on_exitButton_clicked()
{
    close();
}


void PiGuider::on_pushButton_clicked()
{
    lsas.ReadData("settings.cfg","",true); // configuration
    lsas.ReadData("params.cfg","params-default.cfg",false);
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

void PiGuider::SetHardIntervlometerButtonImage(bool on)
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

void PiGuider::SetSoftIntervlometerButtonImage(bool on)
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
