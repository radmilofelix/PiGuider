#ifndef DSLR_H
#define DSLR_H

#include <QWidget>

#include <gphoto2/gphoto2.h>
#include <gphoto2/gphoto2-camera.h>

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

private slots:
    void on_closeButton_clicked();
    void on_enableButton_clicked();
    void on_pushButton_clicked();

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
};

#endif // DSLR_H
