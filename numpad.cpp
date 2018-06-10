#include "numpad.h"
#include "ui_numpad.h"

  NumPad::NumPad(QWidget *parent) :
      QWidget(parent),
      ui(new Ui::NumPad)

{
    ui->setupUi(this);
    connect(ui->pushButtonReturn, SIGNAL(clicked(bool)), this, SLOT(ReturnClickedSlot(bool)));
    inputResult="0";
    busy=false;
    DisplayInput();
}

NumPad::~NumPad()
{
    delete ui;
}

void NumPad::DisplayInput()
{
    if( inputResult.length()>1 && inputResult.at(0) == '0')
    {
        inputResult.remove(0,1);
    }
    ui->label->setText(inputResult);
}

void NumPad::Refresh()
{
    ui->label->setText(inputResult);
}


void ReturnClickedSignal()
{

}

void NumPad::on_pushButton0_clicked()
{
    inputResult+="0";
    DisplayInput();
}

void NumPad::on_pushButton1_clicked()
{
    inputResult+="1";
    DisplayInput();
}

void NumPad::on_pushButton2_clicked()
{
    inputResult+="2";
    DisplayInput();
}

void NumPad::on_pushButton3_clicked()
{
    inputResult+="3";
    DisplayInput();
}

void NumPad::on_pushButton4_clicked()
{
    inputResult+="4";
    DisplayInput();
}

void NumPad::on_pushButton5_clicked()
{
    inputResult+="5";
    DisplayInput();
}

void NumPad::on_pushButton6_clicked()
{
    inputResult+="6";
    DisplayInput();
}

void NumPad::on_pushButton7_clicked()
{
    inputResult+="7";
    DisplayInput();
}

void NumPad::on_pushButton8_clicked()
{
    inputResult+="8";
    DisplayInput();
}

void NumPad::on_pushButton9_clicked()
{
    inputResult+="9";
    DisplayInput();
}

void NumPad::on_pushButtonClear_clicked()
{
    inputResult="0";
    DisplayInput();
}

void NumPad::on_pushButtonReturn_clicked()
{
    busy=false;
    close();
}

void NumPad::ReturnClickedSlot(bool)
{
    emit ReturnClickedSignal();
}
