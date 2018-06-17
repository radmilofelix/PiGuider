#ifndef INTERVALOMETERSOFT_H
#define INTERVALOMETERSOFT_H

#include <QWidget>
#include "loadsaveandsettings.h"
#include "numpad.h"
#include <QElapsedTimer>
//#include "pincontrolpi.h"
#include "dslrcameracontrol.h"




#define ITERATIONS1 13
#define JOBDELAYHOURS1 1
#define JOBDELAYMINUTES1 2
#define JOBDELAYSECONDS1 3
#define RESTHOURS1 4
#define RESTMINUTES1 5
#define RESTSECONDS1 6
#define FOCUSHOURS1 7
#define FOCUSMINUTES1 8
#define FOCUSSECONDS1 9
#define EXPOSUREHOURS1 10
#define EXPOSUREMINUTES1 11
#define EXPOSURESECONDS1 12



namespace Ui {
class IntervalometerSoft;
}

class IntervalometerSoft : public QWidget
{
    Q_OBJECT

public:
    explicit IntervalometerSoft(QWidget *parent = 0);
    ~IntervalometerSoft();
    bool enabled;

    bool editEnabled;
    bool intervalometer1Enabled;
    bool enableVirtualKeyboard;
    NumPad numpad;
    DslrCameraControl dslrCamera;
    int iterations1, iterationsCount1;
    int jobDelayHours1, jobDelayHoursCount1;
    int jobDelayMinutes1, jobDelayMinutesCount1;
    int jobDelaySeconds1, jobDelaySecondsCount1;
    int restHours1, restHoursCount1;
    int restMinutes1, restMinutesCount1;
    int restSeconds1, restSecondsCount1;
    int focusHours1, focusHoursCount1;
    int focusMinutes1, focusMinutesCount1;
    int focusSeconds1, focusSecondsCount1;
    int exposureHours1, exposureHoursCount1;
    int exposureMinutes1, exposureMinutesCount1;
    int exposureSeconds1, exposureSecondsCount1;

    long jobDelayPeriod1;
    long restPeriod1;
    long focusPeriod1;
    long exposurePeriod1;

    long jobDelayPeriodCount1;
    long restPeriodCount1;
    long focusPeriodCount1;
    long exposurePeriodCount1;
    QElapsedTimer timer;
    long oldTime;

    QPixmap changingButtonsPixmap;
    QSize buttonSize;
    LoadSaveAndSettings lsas;


    void Init();
    void UpdateStatus();
    void ReadValues();
    void ValidateInput();
    void DisplayInputValues();
    void DisplayCounterValues();
    void NumPadCaller(int nameOfControl);
    long HmsToLong(int hours, int minutes, int seconds);
    void LongToHMS(long HMS, int *hours, int *minutes, int *seconds);
    void HmsToPeriods();
    void PeriodsToHMS();
    void DecreaseTimer1();
    void DecreaseTimers();
    void FrameMessage(QString message);


    void SetEnableButtonImage(bool on);
    void SetConnectButtonImage(bool on);
    void SetToggleNumpadButtonImage(bool on);

private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void NumpadReturnClicked();
    void on_toggleVirtualNumpadButton_clicked();
    void on_pushButton_clicked();


    void on_exposureSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_exposureMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_exposureHoursLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_focusHoursLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_focusMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_focusSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_restHoursLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_restMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_restSecondsLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_jobDelayHoursLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_jobDelayMinutesLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_jobDelaySecondsLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_iterationsLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_connectButton_clicked();

private:
    Ui::IntervalometerSoft *ui;
};

#endif // INTERVALOMETERSOFT_H
