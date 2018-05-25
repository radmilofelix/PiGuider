#include "imagelabel.h"

ImageLabel::ImageLabel(QWidget *parent)
    : QLabel(parent)
{

}

ImageLabel::~ImageLabel()
{

}

void ImageLabel::mouseMoveEvent(QMouseEvent *event)
{
    this->x = event->x();
    this->y = event->y();
    emit Mouse_Pos();
}

void ImageLabel::mousePressEvent(QMouseEvent *event)
{
    this->x = event->x();
    this->y = event->y();
    emit Mouse_Pressed();
}

void ImageLabel::leaveEvent(QEvent *)
{
    emit Mouse_Left();
}




