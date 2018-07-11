#include "dslr.h"
#include "ui_dslr.h"



#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
#include "globalsettings.h"
#include <time.h>

#include <iostream>
#include <string>


#include <QMouseEvent>
#include "imagelabel.h"

using namespace cv;
using namespace std;



DSLR::DSLR(QWidget *parent) :
    QWidget(parent),
    ui(new Ui::DSLR)
{
    ui->setupUi(this);
    connect(ui->dslrImageLabel, SIGNAL(Mouse_Pos()), this, SLOT(Mouse_current_pos()));
    connect(ui->dslrImageLabel, SIGNAL(Mouse_Pressed()), this, SLOT(Mouse_pressed()));
    connect(ui->dslrImageLabel, SIGNAL(Mouse_Left()), this, SLOT(Mouse_left()));
    connect(&numpad, SIGNAL(ReturnClickedSignal()), this, SLOT(NumpadReturnClicked()));
    buttonSize.setHeight(70);
    buttonSize.setWidth(70);
    cameraFocus=0;
    targetPosition.setX(478/2);
    targetPosition.setY(478/2);
    enabled=false;
    enableShot=false;
    targetSelected=false;
//    lsas.ReadData("settings.cfg","", true);
    if(lsas.captureTethered.value)
        tethered=true;
    else
        tethered=false;
    usbStoreLocation=lsas.usbFlashMemoryPath.value;
    magnification=1;
    eosZoomScale=EOSZOOMSCALEDEFAULT;
    eosZoomWindowWidth=1065;
    eosZoomWindowHeight=710;
    eosZoomPositionX=0;
    eosZoomPositionY=0;
    ui->horizontalGammaSlider->setRange(0, 2000);
    ui->horizontalGammaSlider->setValue(1000);
    ui->horizontalGammaSlider->setVisible(true);
    dgeometry.scaleX=0;
    dgeometry.scaleY=0;
    NewCapture(dslrCamera.fromCamera);
    ui->horizontalZoomSlider->setRange(10, 1000);
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX);
    ui->horizontalZoomSlider->setVisible(true);
    editEnabled=true;
    captureModePreview=true;
    enableConnect=true;
    SetCaptureModeButtonImage(captureModePreview);
    exposureSeconds=1;
    ui->exposureSecondsLineEdit->setText(QString::number((int)exposureSeconds));
}

DSLR::~DSLR()
{
    delete ui;
}

void DSLR::GammaCorrection(const Mat &img, const double gamma_, Mat *result)
{
    CV_Assert(gamma_ >= 0);
    //![changing-contrast-brightness-gamma-correction]
//    Mat img_gamma_corrected;
    Mat lookUpTable(1, 256, CV_8U);
    uchar* p = lookUpTable.ptr();
    for( int i = 0; i < 256; ++i)
        p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);
    *result = img.clone();
    LUT(img, lookUpTable, *result);
    //![changing-contrast-brightness-gamma-correction]
}

void DSLR::NewCapture(bool fromCamera)
{
    if(!fromCamera)
        LoadFromQrc("://media/NightSky.png",IMREAD_COLOR);
    else
        srcImage = imread(dslrCamera.capturedFileName, CV_LOAD_IMAGE_UNCHANGED);
    if(srcImage.empty())
    {
//        ui->labelMessages->setText("Could not open or find camera image");
//        ui->labelMessages->adjustSize();
        return;
    }
    dgeometry.CheckSourceDimensions(srcImage);
    if(dgeometry.scaleX==0)
    {
        dgeometry.init(srcImage.cols, srcImage.rows);
        dgeometry.CropResize(srcImage, &myImage, &processImage);
    }
    else
        dgeometry.CropResize(srcImage, &myImage, &processImage);
//    imwrite("/run/shm/DSLR-SourceImage.jpg", srcImage);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->dslrImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
//    ui->labelMessages->setText(" DSLR Focus Aid ");
//    ui->labelMessages->adjustSize();
    if(fromCamera)
        FrameMessage("DSLR Focus Aid");
    this->repaint();
}

void DSLR::Mouse_current_pos()
{
}

void DSLR::Mouse_pressed()
{
    targetPosition.setX(ui->dslrImageLabel->x);
    targetPosition.setY(ui->dslrImageLabel->y);
    cout << "X: " << ui->dslrImageLabel->x << endl;
    cout << "Y: " << ui->dslrImageLabel->y << endl;
}

void DSLR::Mouse_left()
{

}

void DSLR::NumpadReturnClicked()
{
    ui->exposureSecondsLineEdit->setText(numpad.inputResult);
    exposureSeconds=ui->exposureSecondsLineEdit->text().toInt();
    if(exposureSeconds < 0)
    {
        exposureSeconds=0;
        ui->exposureSecondsLineEdit->setText("0");
    }    if(exposureSeconds > 59)
    {
        exposureSeconds=59;
        ui->exposureSecondsLineEdit->setText("59");
    }
    editEnabled=true;
}



void DSLR::on_closeButton_clicked()
{
    if(dslrCamera.isCamera && dslrCamera.isCameraFile)
        on_connectButton_clicked();
    close();
}

void DSLR::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        SetEnableButtonImage(false);
    }
    else
    {
        if(!dslrCamera.isCamera || !dslrCamera.isCameraFile)
        {
            FrameMessage("Can not enable capture.\nDSLR Camera not connected");
            return;
        }
        enabled=true;
        SetEnableButtonImage(true);
    }
}

void DSLR::on_CaptureImage_clicked()
{
    tethered=true;
    TakeShot(tethered);
}

void DSLR::on_targetButton_clicked()
{
    targetSelected=true;
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, targetPosition.x(), targetPosition.y(), true);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    NewCapture(dslrCamera.fromCamera);
    if(magnification==1)
        ComputeEosZoomOrigin();
    ui->dslrImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888))); // colour
//    cvtColor(myImage, myImage, CV_BGR2RGB);
    imwrite("/run/shm/DSLR-WorkingImage.jpg", myImage);
    this->repaint();
}

void DSLR::on_plusButton_clicked()
{
    //    ui->horizontalSlider->setValue((int)(dgeometry.scaleX*10*1.1));
        dgeometry.Zoom(1.1);
        dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, false);
        GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
//        imwrite("/run/shm/DSLR-WorkingImage.jpg", myImage);
        cvtColor(myImage, myImage, CV_BGR2RGB);
        ui->dslrImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
//        DisplayData();
        ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX*10);
        this->repaint();
}

void DSLR::on_minusButton_clicked()
{
    dgeometry.Zoom(0.9);
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, false);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    imwrite("/run/shm/DSLR-WorkingImage.jpg", myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->dslrImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
//    DisplayData();
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX*10);
    this->repaint();}


void DSLR::on_horizontalZoomSlider_valueChanged(int value)
{
    dgeometry.ZoomAbsolute((float)(value/10));
//    dgeometry.CropResize(srcImage, &myImage, &processImage);
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, dgeometry.relativeTargetX, dgeometry.relativeTargetY, false);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    imwrite("/run/shm/DSLR-WorkingImage.jpg", myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->dslrImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
//    DisplayData();
    this->repaint();}

void DSLR::on_horizontalGammaSlider_valueChanged(int value)
{
}

void DSLR::on_horizontalGammaSlider_sliderReleased()
{
    NewCapture(dslrCamera.fromCamera);
}

void DSLR::on_focusMinus3Button_clicked()
{
    MoveCameraFocus(-2); // near 3
    if(!captureModePreview)
        enableShot=true;
}

void DSLR::on_focusMinus2Button_clicked()
{
    MoveCameraFocus(-3); // near 2
    if(!captureModePreview)
        enableShot=true;
}

void DSLR::on_focusMinus1Button_clicked()
{
    MoveCameraFocus(-4); // near 1
    if(!captureModePreview)
        enableShot=true;
}

void DSLR::on_focusPlus1Button_clicked()
{
    MoveCameraFocus(0); // Far 1
    if(!captureModePreview)
        enableShot=true;
}

void DSLR::on_focusPlus2Button_clicked()
{
    MoveCameraFocus(1); // Far 2
    if(!captureModePreview)
        enableShot=true;
}

void DSLR::on_focusPlus3Button_clicked()
{
    MoveCameraFocus(2); // Far 3
    if(!captureModePreview)
        enableShot=true;
}


void DSLR::FrameMessage(QString message)
{
    message=" "+message+" ";
	ui->labelMessages->setText(message);
    ui->labelMessages->adjustSize();
}


void DSLR::MoveCameraFocus(int value)
{
    if(!dslrCamera.SetCameraFocus(value))
        FrameMessage(dslrCamera.dslrMessage);
    else
        NewCapture(dslrCamera.fromCamera);
}

void DSLR::GetCameraPreview()
{
    if(!dslrCamera.CaptureCameraPreview())
        FrameMessage((dslrCamera.dslrMessage));
    else
        NewCapture(dslrCamera.fromCamera);
}

void DSLR::ComputeEosZoomOrigin()
{
    if(!dslrCamera.isCamera || !dslrCamera.isCameraFile)
        return;
    eosZoomPositionX=dgeometry.sourceTargetX*eosZoomScale-eosZoomWindowWidth/2;
    eosZoomPositionY=dgeometry.sourceTargetY*eosZoomScale-eosZoomWindowHeight/2;
    if(!dslrCamera.SetEosZoomOrigin(eosZoomPositionX, eosZoomPositionY))
    {
        FrameMessage((dslrCamera.dslrMessage));
        return;
    }
}

void DSLR::RefreshData()
{
    if(!enabled)
        return;
    if(captureModePreview)
        GetCameraPreview();
    else
    {
        if(enableShot)
        {
            TakeShot(tethered);
            enableShot=false;
        }
    }
}

void DSLR::LoadFromQrc(QString qrc, int flag)
{
    QFile file(qrc);
    if(file.open(QIODevice::ReadOnly))
    {
        qint64 sz = file.size();
        std::vector<uchar> buf(sz);
        file.read((char*)buf.data(), sz);
        srcImage = imdecode(buf, flag);
    }
}

void DSLR::NumPadCaller()
{
    if(!numpad.busy)// && enableVirtualKeyboard)
    {
        numpad.busy=true;
        numpad.inputResult="0";
        numpad.Refresh();
        numpad.show();
    }
    else
        NumpadReturnClicked();
}

void DSLR::TakeShot(bool isTethered)
{
    if(isTethered)
    {
        if(!dslrCamera.SetCaptureTargetToMemory())
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }
        if(dslrCamera.ShootAndTether("/run/shm/", "DSLR-T-SourceImage", exposureSeconds))
        {
            dslrCamera.fromCamera=true;
            FrameMessage(dslrCamera.capturedFileName);
        }
        else
        {
            FrameMessage(dslrCamera.dslrMessage);
            dslrCamera.fromCamera=false;
        }
    }
    else
    {
        if(!dslrCamera.SetCaptureTargetToSD())
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }

        if(!dslrCamera.ShootAndCapture("/run/shm/", "DSLR-C-SourceImage.jpg", exposureSeconds))
        {
            FrameMessage(dslrCamera.dslrMessage);
            dslrCamera.fromCamera=false;
        }
        else
            dslrCamera.fromCamera=true;
    }
    NewCapture(dslrCamera.fromCamera);
}

void DSLR::SetEnableButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/shutdown.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->enableButton->setIcon(buttonIcon);
    ui->enableButton->setIconSize(buttonSize);
    ui->enableButton->setFixedSize(buttonSize);
}

void DSLR::SetConnectButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/connect-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/connect.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->connectButton->setIcon(buttonIcon);
    ui->connectButton->setIconSize(buttonSize);
    ui->connectButton->setFixedSize(buttonSize);
}

void DSLR::SetX1X10ButtonImage(bool on)
{
    QIcon buttonIcon;
    if(on)
        changingButtonsPixmap.load("://media/icons/tools-512x512/magnify-on.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/magnify.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->x1x10Button->setIcon(buttonIcon);
    ui->x1x10Button->setIconSize(buttonSize);
    ui->x1x10Button->setFixedSize(buttonSize);
}

void DSLR::SetCaptureModeButtonImage(bool preview)
{
    QIcon buttonIcon;
    changingButtonsPixmap.load("://media/icons/tools-512x512/neutralSquareBright.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->captureModeButton->setIcon(buttonIcon);
    ui->captureModeButton->setIconSize(buttonSize);
    ui->captureModeButton->setFixedSize(buttonSize);
    if(preview)
        changingButtonsPixmap.load("://media/icons/tools-512x512/preview.png");
    else
        changingButtonsPixmap.load("://media/icons/tools-512x512/shoot.png");
    buttonIcon.addPixmap(changingButtonsPixmap);
    ui->captureModeButton->setIcon(buttonIcon);
    ui->captureModeButton->setIconSize(buttonSize);
    ui->captureModeButton->setFixedSize(buttonSize);
}


void DSLR::on_resetButton_clicked()
{
    targetSelected=false;
    dgeometry.scaleX=0;
    dgeometry.scaleY=0;
    ui->horizontalGammaSlider->setValue(1000);
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX);
    GetCameraPreview();
    this->repaint();
}

void DSLR::on_connectButton_clicked()
{
    if(!enableConnect)
        return;
    if(dslrCamera.isCamera && dslrCamera.isCameraFile)
    {
        if(!dslrCamera.SetCaptureTargetToSD())
            FrameMessage(dslrCamera.dslrMessage);
        if(!dslrCamera.Disconnect())
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }
        FrameMessage((dslrCamera.dslrMessage));
        SetConnectButtonImage(false);
        enabled=false;
        SetEnableButtonImage(false);
        SetX1X10ButtonImage(false);
    }
    else
    {
        if(!dslrCamera.Connect())
        {
            FrameMessage(dslrCamera.dslrMessage);
        return;
        }
        FrameMessage((dslrCamera.dslrMessage));
        dslrCamera.GetLastCameraFileNumber();
        if(!dslrCamera.SetCaptureTargetToSD())
//        if(!dslrCamera.SetCaptureTarget(dslrCamera.canonCamera, dslrCamera.canonContext, "Internal RAM"))
            FrameMessage(dslrCamera.dslrMessage);
        SetConnectButtonImage(true);
        GetCameraPreview();
    }
}

void DSLR::on_x1x10Button_clicked()
{
    switch (magnification)
    {
    case 1:
        magnification=10;
        eosZoomScale=EOSZOOMSCALEDEFAULT/1.25;
        if(!dslrCamera.SetMagnification("10"))
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }
        GetCameraPreview();
        GetCameraPreview();
        SetX1X10ButtonImage(true);
        break;
/*
    // magnification X5 seems not to work
    case 5:
        magnification=5;
        if(!dslrCamera.SetMagnification("5"))
                FrameMessage(dslrCamera.dslrMessage);
        GetCameraPreview();
        GetCameraPreview();
        SetX1X10ButtonImage(true);
        break;
*/
    default:
        magnification=1;
        eosZoomScale=EOSZOOMSCALEDEFAULT;
        if(!dslrCamera.SetMagnification("1"))
        {
            FrameMessage(dslrCamera.dslrMessage);
            return;
        }
        GetCameraPreview();
        GetCameraPreview();
        SetX1X10ButtonImage(false);
        break;
    }
}

void DSLR::on_onButton_clicked()
{
    if(!dslrCamera.SetCaptureTargetToSD())
    {
        FrameMessage(dslrCamera.dslrMessage);
        return;
    }
    if(!dslrCamera.ShootOn())
        FrameMessage(dslrCamera.dslrMessage);
}

void DSLR::on_offButton_clicked()
{
    if(!dslrCamera.ShootRelease())
        FrameMessage(dslrCamera.dslrMessage);
}

void DSLR::on_testButton_clicked()
{
    NewCapture(true);
//    if(!dslrCamera.GetLastCameraFileNumber())
//        FrameMessage(dslrCamera.dslrMessage);
//    tethered=false;
//    TakeShot(tethered);
}

void DSLR::on_exposureSecondsLineEdit_cursorPositionChanged(int arg1, int arg2)
{
    if(editEnabled)
    {
        editEnabled=false;
        NumPadCaller();
    }
}

void DSLR::on_captureModeButton_clicked()
{
    if(!dslrCamera.isCamera || !dslrCamera.isCameraFile)
    {
        FrameMessage("Can not change capture mode.\nDSLR Camera not connected");
        return;
    }
    if(captureModePreview)
    {
        captureModePreview=false;
        SetCaptureModeButtonImage(captureModePreview);
        dslrCamera.Disconnect();
        dslrCamera.Connect();
    }
    else
    {
        captureModePreview=true;
        SetCaptureModeButtonImage(captureModePreview);
        dslrCamera.Disconnect();
        dslrCamera.Connect();
    }
}
