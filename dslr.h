#ifndef DSLR_H
#define DSLR_H

#include <QWidget>
#include "dslrcameracontrol.h"

#include <gphoto2/gphoto2.h>
#include <gphoto2/gphoto2-camera.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "displaygeometry.h"
#include "loadsaveandsettings.h"
#include "numpad.h"

#define EOSZOOMSCALEDEFAULT 6

namespace Ui
{
    class DSLR;
}

class DSLR : public QWidget
{
    Q_OBJECT

public:
    explicit DSLR(QWidget *parent = 0);
    ~DSLR();
    DslrCameraControl dslrCamera;
    bool enabled;
    bool targetSelected;
    bool tethered;
    bool enableShot;
    int cameraFocus;
    int magnification;
    float eosZoomScale;
    double eosZoomPositionX, eosZoomPositionY;
    double eosZoomWindowWidth, eosZoomWindowHeight;
    QString usbStoreLocation;
    QString eosZoomPositionString;
    cv::Mat myImage, srcImage, processImage;
    DisplayGeometry dgeometry;
    NumPad numpad;
    QPoint targetPosition;
    QString dslrMessage;
    QPixmap changingButtonsPixmap;
    QSize buttonSize;
    LoadSaveAndSettings lsas;
    int exposureSeconds;
    bool editEnabled;
    bool captureModePreview;
    bool enableConnect;

    void GammaCorrection(const cv::Mat &img, const double gamma_, cv::Mat *result);
    void NewCapture(bool fromCamera);
    void FrameMessage(QString message);
    void MoveCameraFocus(int value);
    void GetCameraPreview();
    void ComputeEosZoomOrigin();
    void RefreshData();
    void LoadFromQrc(QString qrc, int flag);
    void NumPadCaller();
    void TakeShot(bool isTethered);

    void SetEnableButtonImage(bool on);
    void SetConnectButtonImage(bool on);
    void SetX1X10ButtonImage(bool on);
    void SetCaptureModeButtonImage(bool preview);

private slots:
    void Mouse_current_pos();
    void Mouse_pressed();
    void Mouse_left();
    void NumpadReturnClicked();

    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void on_CaptureImage_clicked();
    void on_targetButton_clicked();
    void on_plusButton_clicked();
    void on_minusButton_clicked();
    void on_horizontalZoomSlider_valueChanged(int value);
    void on_horizontalGammaSlider_valueChanged(int value);
    void on_horizontalGammaSlider_sliderReleased();
    void on_focusMinus3Button_clicked();
    void on_focusMinus2Button_clicked();
    void on_focusMinus1Button_clicked();
    void on_focusPlus1Button_clicked();
    void on_focusPlus2Button_clicked();
    void on_focusPlus3Button_clicked();
    void on_resetButton_clicked();
    void on_connectButton_clicked();
    void on_x1x10Button_clicked();

    void on_onButton_clicked();
    void on_offButton_clicked();


    void on_testButton_clicked();
    void on_exposureSecondsLineEdit_cursorPositionChanged(int arg1, int arg2);
    void on_captureModeButton_clicked();

private:
    Ui::DSLR *ui;

};

#endif // DSLR_H
