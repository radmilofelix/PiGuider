#include "intervalometerhard.h"
#include "ui_intervalometerhard.h"

IntervalometerHard::IntervalometerHard(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::IntervalometerHard)
{
    ui->setupUi(this);
    enabled=false;
    QPixmap image("media/icons/tools-32x32/led-red.png");
    ui->ledLabel->setPixmap(image);
}

IntervalometerHard::~IntervalometerHard()
{
    delete ui;
}

void IntervalometerHard::updateStatus()
{
    if(enabled)
    {
        QPixmap image("media/icons/tools-32x32/led-green.png");
        ui->ledLabel->setPixmap(image);
    }
    else
    {
        QPixmap image("media/icons/tools-32x32/led-red.png");
        ui->ledLabel->setPixmap(image);
    }
}


void IntervalometerHard::on_closeButton_clicked()
{
    close();
}

void IntervalometerHard::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        QPixmap image("media/icons/tools-32x32/led-red.png");
        ui->ledLabel->setPixmap(image);
    }
    else
    {
        enabled=true;
        QPixmap image("media/icons/tools-32x32/led-green.png");
        ui->ledLabel->setPixmap(image);
    }
}
