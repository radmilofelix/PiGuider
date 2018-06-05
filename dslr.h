#ifndef DSLR_H
#define DSLR_H

#include <QWidget>

#include <gphoto2/gphoto2.h>
#include <gphoto2/gphoto2-camera.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/features2d/features2d.hpp>
#include "displaygeometry.h"


namespace Ui {
class DSLR;
}

class DSLR : public QWidget
{
    Q_OBJECT

public:
    explicit DSLR(QWidget *parent = 0);
    ~DSLR();
//    QMouseEvent mevt;
    bool enabled;
    bool targetSelected;
    bool fromCamera;
    int cameraFocus;
    bool focusTowardsPlus;
    cv::Mat myImage, srcImage, processImage;
    DisplayGeometry dgeometry;
    Camera	*canonCamera;
    GPContext *canonContext;
    CameraFile *canonFile;
    bool isCamera;
    bool isCameraFile;
    QPoint targetPosition;
    QString dslrMessage;
    void GammaCorrection(const cv::Mat &img, const double gamma_, cv::Mat *result);
    void NewCapture(bool fromCamera);
    void CameraGrab();
    void CameraRelease();
    void FrameMessage(QString message);
    void MoveCameraFocus(int value);
    void CaptureCameraPreview();

private slots:
    void Mouse_current_pos();
    void Mouse_pressed();
    void Mouse_left();

    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void on_pushButton_clicked();

    void on_CapturePreviewButton_clicked();
    void on_CaptureImage_clicked();

    void on_grabButton_clicked();
    void on_releaseButton_clicked();

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

private:
    Ui::DSLR *ui;

    static void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn);
//    static void errordumper(GPLogLevel level, const char *domain, const char *str, void *data);
    int canon_enable_capture (Camera *camera, int onoff, GPContext *context);
    int set_config_value_string (Camera *camera, const char *key, const char *val, GPContext *context);
    int get_config_value_string (Camera *camera, const char *key, char **str, GPContext *context);
    int camera_manual_focus (Camera *camera, int xx, GPContext *context);
    int camera_auto_focus(Camera *camera, GPContext *context, int onoff);
//    int camera_eosviewfinder(Camera *camera, GPContext *context, int onoff);
    static int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child);
    void camera_tether(Camera *camera, GPContext *context,  char *fn);
};

#endif // DSLR_H
