#ifndef DSLRCAMERACONTROL_H
#define DSLRCAMERACONTROL_H

#include <gphoto2/gphoto2.h>
#include <gphoto2/gphoto2-camera.h>
#include <QString>

class DslrCameraControl
{
public:
    DslrCameraControl();
    ~DslrCameraControl();

    bool isCamera;
    bool isCameraFile;
    bool fromCamera;
    int cameraFocus;
    QString dslrMessage;
    Camera	*canonCamera;
    GPContext *canonContext;
    CameraFile *canonFile;
    char *sourceFileName;
    char *captureFileExtension;

    void CameraGrab();
    void CameraRelease();
    int SetCameraFocus(int value);
    int CaptureCameraPreview();
    int SetEosZoomOrigin(double eosZoomPositionX, double eosZoomPositionY);
    size_t RunUnixCommandStoreOutput(char * param, char * result, int resultdim);
    void KillGvfsdProcess();
    int SetMagnification(char* magnification);
    int ShootOn();
    int ShootRelease();
    void GetCaptureFileExtension(char *inputString);
    int ShootAndCapture(char *path, char *fileName, int exposureTime);

private:
    int GetConfigValueString(Camera *camera, const char *key, char **str, GPContext *context);
    int SetConfigValueString (Camera *camera, const char *key, const char *val, GPContext *context);
    int CameraManualFocus (Camera *camera, int xx, GPContext *context);
    static int LookupWidget(CameraWidget*widget, const char *key, CameraWidget **child);
    void CameraTether(Camera *camera, GPContext *context,  char *destinationFolder, char *fileName);
//    static void capture_to_file(Camera *canon, GPContext *canoncontext, char *fn);
//    static void errordumper(GPLogLevel level, const char *domain, const char *str, void *data);
//    int canon_enable_capture (Camera *camera, int onoff, GPContext *context);
//    int camera_auto_focus(Camera *camera, GPContext *context, int onoff);
//    int camera_eosviewfinder(Camera *camera, GPContext *context, int onoff);
//    void set_capturetarget(Camera *canon, GPContext *canoncontext);

};

#endif // DSLRCAMERACONTROL_H
