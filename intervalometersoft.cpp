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
    enabled=false;
    buttonSize.setHeight(70);
    buttonSize.setWidth(70);
}

IntervalometerSoft::~IntervalometerSoft()
{
    delete ui;
}

void IntervalometerSoft::updateStatus()
{
    if(enabled)
    {
        SetEnableButtonImage(true);
    }
    else
    {
        SetEnableButtonImage(false);
    }
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


void IntervalometerSoft::on_closeButton_clicked()
{
    close();
}

void IntervalometerSoft::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        SetEnableButtonImage(false);
    }
    else
    {
        enabled=true;
        SetEnableButtonImage(true);
    }
}

/*
void IntervalometerSoft::setLedColour(std::string filename)
{
    Mat myImage, srcImage;
    srcImage = imread(filename, CV_LOAD_IMAGE_UNCHANGED);
    int newx=ui->ledLabel->width();
//    int newx=100;
    int newy=srcImage.rows*newx/srcImage.cols;
//    int newy=100;
    cv::Size mSize(newx,newy);//the dst image size,e.g.100x100
    cv::resize( srcImage, myImage, mSize);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->ledLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
//    ui->ledLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_ARGB32_Premultiplied)));
}
*/


