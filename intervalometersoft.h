#ifndef INTERVALOMETERSOFT_H
#define INTERVALOMETERSOFT_H

#include <QWidget>

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
    QPixmap changingButtonsPixmap;
    QSize buttonSize;
//    void setLedColour(std::string filename);
    void updateStatus();
    void SetEnableButtonImage(bool on);

private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();

private:
    Ui::IntervalometerSoft *ui;
};

#endif // INTERVALOMETERSOFT_H
