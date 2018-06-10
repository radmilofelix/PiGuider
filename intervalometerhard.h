#ifndef INTERVALOMETERHARD_H
#define INTERVALOMETERHARD_H
#include "numpad.h"
#include <QWidget>
#include <QElapsedTimer>
#include "pincontrolpi.h"

#define ITERATIONS1 25
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

#define ITERATIONS2 26
#define JOBDELAYHOURS2 13
#define JOBDELAYMINUTES2 14
#define JOBDELAYSECONDS2 15
#define RESTHOURS2 16
#define RESTMINUTES2 17
#define RESTSECONDS2 18
#define FOCUSHOURS2 19
#define FOCUSMINUTES2 20
#define FOCUSSECONDS2 21
#define EXPOSUREHOURS2 22
#define EXPOSUREMINUTES2 23
#define EXPOSURESECONDS2 24

namespace Ui {
class IntervalometerHard;
}

class IntervalometerHard : public QWidget
{
    Q_OBJECT

public:
    explicit IntervalometerHard(QWidget *parent = 0);
    ~IntervalometerHard();
    bool enabled;
    bool editEnabled;
    bool intervalometer1Enabled;
    bool intervalometer2Enabled;
    bool enableVirtualKeyboard;
    NumPad numpad;
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

    int iterations2, iterationsCount2;
    int jobDelayHours2, jobDelayHoursCount2;
    int jobDelayMinutes2, jobDelayMinutesCount2;
    int jobDelaySeconds2, jobDelaySecondsCount2;
    int restHours2, restHoursCount2;
    int restMinutes2, restMinutesCount2;
    int restSeconds2, restSecondsCount2;
    int focusHours2, focusHoursCount2;
    int focusMinutes2, focusMinutesCount2;
    int focusSeconds2, focusSecondsCount2;
    int exposureHours2, exposureHoursCount2;
    int exposureMinutes2, exposureMinutesCount2;
    int exposureSeconds2, exposureSecondsCount2;

    long jobDelayPeriod1;
    long restPeriod1;
    long focusPeriod1;
    long exposurePeriod1;
    long jobDelayPeriod2;
    long restPeriod2;
    long focusPeriod2;
    long exposurePeriod2;

    long jobDelayPeriodCount1;
    long restPeriodCount1;
    long focusPeriodCount1;
    long exposurePeriodCount1;
    long jobDelayPeriodCount2;
    long restPeriodCount2;
    long focusPeriodCount2;
    long exposurePeriodCount2;

    PinControlPi pincontrol;
    QPixmap ledImage;
    QElapsedTimer timer;
    long oldTime;
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
    void DecreaseTimer2();
    void DecreaseTimers();

private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void NumpadReturnClickedSlot();

    void on_pushButton_clicked();

    void on_intervalometer1Button_clicked();
    void on_intervalometer2Button_clicked();

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

    void on_exposureSecondsLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_exposureMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_exposureHoursLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_focusHoursLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_focusMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_focusSecondsLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_restHoursLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_restMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_restSecondsLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_jobDelayHoursLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_jobDelayMinutesLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_jobDelaySecondsLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_iterationsLineEdit1_cursorPositionChanged(int arg1, int arg2);
    void on_iterationsLineEdit2_cursorPositionChanged(int arg1, int arg2);
    void on_toggleVirtualNumpadButton_clicked();

private:
    Ui::IntervalometerHard *ui;
};

#endif // INTERVALOMETERHARD_H
