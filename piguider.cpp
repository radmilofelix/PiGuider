#include "piguider.h"
#include "ui_piguider.h"
#include <QTimer>
#include <QTime>


#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include "globalsettings.h"



using namespace cv;


PiGuider::PiGuider(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::PiGuider)
{
    //  Remove main window frame
    this->setWindowFlags( Qt::FramelessWindowHint );
    ui->setupUi(this);
//    QPixmap image("media/NightSky.png");
//    ui->mainImageLabel->setPixmap(image);
    dslr_status=false;
    guider_status=false;
    intervalometersoft_status=false;
    intervalometerhard_status=false;

    QPixmap image("media/icons/tools-16x16/led-red.png");
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
    updateStatus();
}

void PiGuider::updateStatus()
{
    QPixmap ledred("media/icons/tools-16x16/led-red.png");
    QPixmap ledgreen("media/icons/tools-16x16/led-green.png");

    if( dslr.enabled != dslr_status)
    {
        dslr_status=dslr.enabled;
        if(dslr.enabled)
            ui->ledLabelDSLR->setPixmap(ledgreen);
        else
            ui->ledLabelDSLR->setPixmap(ledred);
    }
    if( guider.enabled != guider_status )
    {
        guider_status=guider.enabled;
        if(guider.enabled)
            ui->ledLabelGuider->setPixmap(ledgreen);
        else
        {
            ui->ledLabelGuider->setPixmap(ledred);
//            QPixmap image("media/NightSky.png");
//            ui->mainImageLabel->setPixmap(image);
        }
    }
    if( intervalometersoft.enabled != intervalometersoft_status )
    {
        intervalometersoft_status=intervalometersoft.enabled;
        if(intervalometersoft.enabled)
        {
            ui->ledLabelSoftIntervalometer->setPixmap(ledgreen);
            intervalometerhard.enabled=false;
            intervalometerhard.updateStatus();
        }
        else
            ui->ledLabelSoftIntervalometer->setPixmap(ledred);
    }
    if( intervalometerhard.enabled != intervalometerhard_status)
    {
        intervalometerhard_status=intervalometerhard.enabled;
        if(intervalometerhard.enabled)
        {
            ui->ledLabelHardIntervalometer->setPixmap(ledgreen);
            intervalometersoft.enabled=false;
            intervalometersoft.updateStatus();
            ui->ledLabelSoftIntervalometer->setPixmap(ledred);
        }
        else
            ui->ledLabelHardIntervalometer->setPixmap(ledred);
    }
    if(guider.enabled)
    {
        guider.RefreshData();
        QPixmap guideimage("/run/shm/mat.jpg");
        ui->mainImageLabel->setPixmap(guideimage);
    }
//    else
//    {
//        QPixmap image("media/NightSky.png");
//        ui->mainImageLabel->setPixmap(image);
//    }
}

void PiGuider::on_exitButton_clicked()
{
    close();
}


void PiGuider::on_pushButton_clicked()
{
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
        guider.show();
}

void PiGuider::on_focusButton_clicked()
{
    dslr.show();
    QPixmap image("media/focus-img.png");
    ui->mainImageLabel->setPixmap(image);
}



void PiGuider::on_intervalometerSoftButton_clicked()
{
    intervalometersoft.show();
}

void PiGuider::on_intervalometerHardButton_clicked()
{
    intervalometerhard.show();
}
