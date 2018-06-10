#ifndef PIGUIDER_H
#define PIGUIDER_H

#include <QMainWindow>

#include "guider.h"
#include "dslr.h"
#include "intervalometerhard.h"
#include "intervalometersoft.h"

namespace Ui {
class PiGuider;
}

class PiGuider : public QMainWindow
{
    Q_OBJECT

public:
    explicit PiGuider(QWidget *parent = 0);
    ~PiGuider();

private slots:
    void UpdateTime();
    void on_exitButton_clicked();
    void on_guideButton_clicked();
    void on_focusButton_clicked();
    void on_pushButton_clicked();
    void on_intervalometerSoftButton_clicked();
    void on_intervalometerHardButton_clicked();

private:
    Ui::PiGuider *ui;
    QTimer *timerMain;
    DSLR dslr;
    Guider guider;
    IntervalometerSoft intervalometersoft;
    IntervalometerHard intervalometerhard;
    bool buttonpressed;
    bool dslr_status, guider_status, intervalometersoft_status, intervalometerhard_status;
    void UpdateStatus();

};

#endif // PIGUIDER_H
