#ifndef IMAGELABEL_H
#define IMAGELABEL_H

#include <QLabel>
#include <QWidget>
#include <QMouseEvent>
#include <QDebug>
#include <QEvent>


namespace Ui {
class ImageLabel;
}

class ImageLabel : public QLabel
 {
    Q_OBJECT
public:
    ImageLabel(QWidget *parent = 0);
    virtual ~ImageLabel();
    int x, y;
    void mouseMoveEvent(QMouseEvent *event);
    void mousePressEvent(QMouseEvent *event);
    void leaveEvent(QEvent *);

signals:
    void Mouse_Pressed();
    void Mouse_Pos();
    void Mouse_Left();
};

#endif // IMAGELABEL_H
