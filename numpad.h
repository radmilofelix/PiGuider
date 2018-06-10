#ifndef NUMPAD_H
#define NUMPAD_H

#include <QWidget>

namespace Ui {
class NumPad;
}

class NumPad : public QWidget
{
    Q_OBJECT

public:
    explicit NumPad(QWidget *parent = 0);
    ~NumPad();
    QString inputResult;
    int callerName;
    void DisplayInput();
    bool busy;
    void Refresh();

signals:
    void ReturnClickedSignal();

private slots:
    void on_pushButton0_clicked();
    void on_pushButton1_clicked();
    void on_pushButton2_clicked();
    void on_pushButton3_clicked();
    void on_pushButton4_clicked();
    void on_pushButton5_clicked();
    void on_pushButton6_clicked();
    void on_pushButton7_clicked();
    void on_pushButton8_clicked();
    void on_pushButton9_clicked();
    void on_pushButtonClear_clicked();
    void on_pushButtonReturn_clicked();
    void ReturnClickedSlot(bool);

private:
    Ui::NumPad *ui;
};

#endif // NUMPAD_H
