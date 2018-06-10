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
    QPixmap image("media/icons/tools-32x32/led-red.png");
    ui->ledLabel->setPixmap(image);
}

IntervalometerSoft::~IntervalometerSoft()
{
    delete ui;
}

void IntervalometerSoft::updateStatus()
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


void IntervalometerSoft::on_closeButton_clicked()
{
    close();
}

void IntervalometerSoft::on_enableButton_clicked()
{
    if(enabled)
    {//://media/icons/tools-512x512/plus.png
        enabled=false;
        QPixmap image("media/icons/tools-32x32/led-red.png");
        ui->ledLabel->setPixmap(image);
        QPixmap pixmap("://media/icons/tools-512x512/plus.png");
        QIcon ButtonIcon(pixmap);
        ui->enableButton->setIcon(ButtonIcon);
        QSize msize;
        msize.setHeight(100);
        msize.setWidth(100);
        ui->enableButton->setIconSize(msize);
        ui->enableButton->setFixedSize(msize);
    }
    else
    {
        enabled=true;
        QPixmap image("media/icons/tools-32x32/led-green.png");
        ui->ledLabel->setPixmap(image);
        QPixmap pixmap("://media/icons/tools-512x512/minus.png");
        QIcon ButtonIcon(pixmap);
        ui->enableButton->setIcon(ButtonIcon);
        QSize msize;
        msize.setHeight(100);
        msize.setWidth(100);
        ui->enableButton->setIconSize(msize);
        ui->enableButton->setFixedSize(msize);
    }
}

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
