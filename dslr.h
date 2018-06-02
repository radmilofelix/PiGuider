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
#include "cropandresize.h"



namespace Ui {
class DSLR;
}

class DSLR : public QWidget
{
    Q_OBJECT

public:
    explicit DSLR(QWidget *parent = 0);
    ~DSLR();
    bool enabled;
    bool targetSelected;
    cv::Mat myImage, srcImage, processImage;
    CropAndResize cropresize;
    void NewCapture();
    void LoadUnchanged();

private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void on_pushButton_clicked();

    void on_CapturePreviewButton_clicked();

    void on_CaptureImage_clicked();

private:
    Ui::DSLR *ui;
    static void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn);
//    static void errordumper(GPLogLevel level, const char *domain, const char *str, void *data);
    int canon_enable_capture (Camera *camera, int onoff, GPContext *context);
    int set_config_value_string (Camera *camera, const char *key, const char *val, GPContext *context);
    int get_config_value_string (Camera *camera, const char *key, char **str, GPContext *context);
    int camera_manual_focus (Camera *camera, int xx, GPContext *context);
    int camera_auto_focus(Camera *camera, GPContext *context, int onoff);
    int camera_eosviewfinder(Camera *camera, GPContext *context, int onoff);
    static int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child);
    static void camera_tether(Camera *camera, GPContext *context,  char *fn);
};

#endif // DSLR_H
