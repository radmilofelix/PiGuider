#ifndef INTERVALOMETERHARD_H
#define INTERVALOMETERHARD_H

#include <QWidget>

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
    void updateStatus();

private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();
private:
    Ui::IntervalometerHard *ui;
};

#endif // INTERVALOMETERHARD_H
