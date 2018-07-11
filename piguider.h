#ifndef PIGUIDER_H
#define PIGUIDER_H

#include <QMainWindow>

#include "guider.h"
#include "dslr.h"
#include "intervalometerhard.h"
#include "intervalometersoft.h"
#include "loadsaveandsettings.h"

#define YEAR 1
#define MONTH 2
#define DAY 3
#define HOUR 4
#define MINUTE 6


namespace Ui {
class PiGuider;
}

class PiGuider : public QMainWindow
{
    Q_OBJECT

public:
    explicit PiGuider(QWidget *parent = 0);
    ~PiGuider();
    QPixmap changingButtonsPixmap;
    QSize buttonSize;
    NumPad numpad;
    void NumPadCaller(int nameOfControl);
    void ReadValues();
    void ValidateInput();
    void ReadTime();
    void SetTime();
    QPixmap image;

private slots:
    void UpdateTime();
    void on_exitButton_clicked();
    void on_guideButton_clicked();
    void on_focusButton_clicked();
    void on_pushButton_clicked();
    void on_intervalometerSoftButton_clicked();
    void on_intervalometerHardButton_clicked();
    void SetGuiderButtonImage(bool on);
    void SetFocusButtonImage(bool on);
    void SetHardIntervalometerButtonImage(bool on);
    void SetSoftIntervalometerButtonImage(bool on);
    void NumpadReturnClicked();
    void UpdateGuideInfo();
    void UpdateIntervalometerInfo(bool hardIv);

    void on_yearLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_monthLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_dayLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_hoursLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_minutesLineEdit_cursorPositionChanged(int arg1, int arg2);

private:
    Ui::PiGuider *ui;
    QTimer *timerMain;
    LoadSaveAndSettings lsas;
    DSLR dslr;
    Guider guider;
    IntervalometerSoft intervalometersoft;
    IntervalometerHard intervalometerhard;
    bool buttonpressed, editEnabled;
    bool dslr_status, guider_status, intervalometersoft_status, intervalometerhard_status;
    int raGuideStatus, declGuideStatus;
    int year, month, day, hours, minutes;
    int readTimeCounter;
    void UpdateStatus();

};

#endif // PIGUIDER_H
