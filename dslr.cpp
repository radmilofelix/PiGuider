#include "dslr.h"
#include "ui_dslr.h"



#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdarg.h>
//#include <string.h>
#include "globalsettings.h"

#include <iostream>
#include <string>

//#include <chrono>
//#include <math.h>

/*
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/imgproc/imgproc.hpp>
*/

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

    isCamera=false;
    isCameraFile=false;
    cameraFocus=0;
//    pi=3.14159265358979323846;
    targetPosition.setX(478/2);
    targetPosition.setY(478/2);
//    ui->setupUi(this);
    enabled=false;
    targetSelected=false;
    fromCamera=false;
    QPixmap image("media/icons/tools-32x32/led-red.png");
    ui->ledLabel->setPixmap(image);
//    image.load("media/NightSky.png");
//    ui->dslrImageLabel->setPixmap(image);
    ui->horizontalGammaSlider->setRange(0, 2000);
    ui->horizontalGammaSlider->setValue(1000);
    ui->horizontalGammaSlider->setVisible(true);
    dgeometry.scaleX=0;
    dgeometry.scaleY=0;
    NewCapture(fromCamera);
    ui->horizontalZoomSlider->setRange(10, 1000);
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX);
    ui->horizontalZoomSlider->setVisible(true);
    image.load("media/icons/tools-16x16/led-red.png");
    ui->resetLedLabel->setPixmap(image);
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
        srcImage = imread("media/NightSky.png", CV_LOAD_IMAGE_UNCHANGED);
    else
        srcImage = imread("/run/shm/DSLR-SourceImage.jpg", CV_LOAD_IMAGE_UNCHANGED);
    if(srcImage.empty())
    {
        ui->labelMessages->setText("Could not open or find camera image");
        ui->labelMessages->adjustSize();
        return;
    }
    if(dgeometry.scaleX==0)
    {
        dgeometry.init(srcImage.cols, srcImage.rows);
        dgeometry.DisplayGeometryData("DSLR-after init");
        dgeometry.CropResize(srcImage, &myImage, &processImage);
    }
    else
        dgeometry.CropResize(srcImage, &myImage, &processImage);
//    imwrite("/run/shm/DSLR-SourceImage.jpg", srcImage);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    cvtColor(myImage, myImage, CV_BGR2RGB);
    ui->dslrImageLabel->setPixmap(QPixmap::fromImage(QImage(myImage.data, myImage.cols, myImage.rows, myImage.step, QImage::Format_RGB888)));
    ui->labelMessages->setText(" DSLR Focus Aid ");
    ui->labelMessages->adjustSize();
    this->repaint();
}

void DSLR::CameraGrab()
{
    dslrMessage="";
    int	retval;
    if(!isCamera)
    {
        ui->labelMessages->setText(" Initialising camera... \nTakes about 10 seconds.");
        ui->labelMessages->adjustSize();
        canonContext = gp_context_new();
        gp_camera_new(&canonCamera);
        retval = gp_camera_init(canonCamera, canonContext);
        if (retval != GP_OK)
        {
            QString rezMessage="Could not initialise camera.\nError code: ";
            rezMessage+=QString::number(retval);
            ui->labelMessages->setText(rezMessage);
            ui->labelMessages->adjustSize();
            return;
            }
        isCamera=true;
        dslrMessage+="Camera initialised.";
    }
    else
        dslrMessage+="Camera already initialised.";
    if(!isCameraFile)
    {
        retval = gp_file_new(&canonFile);
        if (retval != GP_OK)
        {
            QString rezMessage="Could not create CameraFile object.\nError code: ";
            rezMessage+=QString::number(retval);
            ui->labelMessages->setText(rezMessage);
            ui->labelMessages->adjustSize();
            return;
        }
        isCameraFile=true;
        dslrMessage+="\nCamera file created.";
    }
    else
    {
        dslrMessage+="\nCamera file already exists.";
        ui->labelMessages->adjustSize();
    }
    ui->labelMessages->setText(dslrMessage);
    ui->labelMessages->adjustSize();
    if(isCamera && isCameraFile)
        fromCamera=true;
    else
        fromCamera=false;
}

void DSLR::CameraRelease()
{
    dslrMessage="";
    if(isCameraFile)
    {
        gp_file_unref(canonFile);
        isCameraFile=false;
        dslrMessage+="Camera file released.\n";
    }
    else
        dslrMessage+="Camera file already released.";
    if(isCamera)
    {
        gp_camera_exit(canonCamera, canonContext);
        gp_camera_unref(canonCamera);
        isCamera=false;
        dslrMessage+="Camera object released";
    }
    else
        dslrMessage+="\nCamera object already released.";
    ui->labelMessages->setText(dslrMessage);
    ui->labelMessages->adjustSize();
    if(isCamera && isCameraFile)
        fromCamera=true;
    else
        fromCamera=false;
}


void DSLR::Mouse_current_pos()
{
//    cout << "X: " << ui->dslrImageLabel->x << endl;
//    cout << "Y: " << ui->dslrImageLabel->y << endl;
}

void DSLR::Mouse_pressed()
{
    targetPosition.setX(ui->dslrImageLabel->x);
    targetPosition.setY(ui->dslrImageLabel->y);
}

void DSLR::Mouse_left()
{

}

void DSLR::on_closeButton_clicked()
{
    close();
}

void DSLR::on_enableButton_clicked()
{
    if(enabled)
    {
        enabled=false;
        QPixmap image("media/icons/tools-32x32/led-red.png");
        ui->ledLabel->setPixmap(image);

    }
    else
    {
        enabled=true;
        QPixmap image("media/icons/tools-32x32/led-green.png");
        ui->ledLabel->setPixmap(image);
    }
}


int DSLR::_lookup_widget(CameraWidget *widget, const char *key, CameraWidget **child)
{
    int ret;
    ret = gp_widget_get_child_by_name (widget, key, child);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widget, key, child);
    return ret;
}


void DSLR::camera_tether(Camera *camera, GPContext *context,  char *fn)
{
    int fd, retval;
//    CameraFile *file;
    CameraEventType	evttype;
    CameraFilePath	*path;
    void	*evtdata;

    printf("Tethering...\n");

    while (1)
    {
        retval = gp_camera_wait_for_event (camera, 1000, &evttype, &evtdata, context);
        if (retval != GP_OK)
            break;
        char saveFile[500];
        strcpy (saveFile,"/run/shm/");
        switch (evttype) {

        case GP_EVENT_FILE_ADDED:
            path = (CameraFilePath*)evtdata;
            strcat(saveFile,path->name);
            printf("File added on the camera: %s/%s\n", path->folder, path->name);

//            if(fn)
//                fd = open(fn, O_CREAT | O_WRONLY, 0644);
//            else
//                fd = open(fn, O_CREAT | O_WRONLY, 0644);
            fd = open (saveFile, O_CREAT | O_WRONLY, 0644);
            retval = gp_file_new_from_fd(&canonFile, fd);
            printf("  Downloading %s...\n", path->name);
            retval = gp_camera_file_get(camera, path->folder, path->name,
                     GP_FILE_TYPE_NORMAL, canonFile, context);

//            printf("  Deleting %s on camera...\n", path->name);
//            retval = gp_camera_file_delete(camera, path->folder, path->name, context);
//            gp_file_free(file);
            free(evtdata);

            return;
            break;

        case GP_EVENT_FOLDER_ADDED:
            path = (CameraFilePath*)evtdata;
            printf("Folder added on camera: %s / %s\n", path->folder, path->name);
            free(evtdata);
            break;
//        case GP_EVENT_FILE_CHANGED:
//            path = (CameraFilePath*)evtdata;
//            printf("File changed on camera: %s / %s\n", path->folder, path->name);
//            free(evtdata);
//            break;
        case GP_EVENT_CAPTURE_COMPLETE:
            printf("Capture Complete.\n");
            break;
        case GP_EVENT_TIMEOUT:
            printf("Timeout.\n");
            break;
        case GP_EVENT_UNKNOWN:
            if (evtdata) {
                printf("Unknown event: %s.\n", (char*)evtdata);
            } else {
                printf("Unknown event.\n");
            }
            break;
        default:
            printf("Type %d?\n", evttype);
            break;
        }
    }
}


/*
// calls the Nikon DSLR or Canon DSLR autofocus method.
int DSLR::camera_eosviewfinder(Camera *camera, GPContext *context, int onoff) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret,val;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, "eosviewfinder", &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup 'eosviewfinder' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_TOGGLE:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not get widget value: %d\n", ret);
        goto out;
    }
    val = onoff;
    ret = gp_widget_set_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set widget value to 1: %d\n", ret);
        goto out;
    }

    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set config tree to eosviewfinder: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}

*/

int DSLR::camera_auto_focus(Camera *camera, GPContext *context, int onoff) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret,val;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, "autofocusdrive", &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup 'autofocusdrive' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_TOGGLE:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not get widget value: %d\n", ret);
        goto out;
    }

    val = onoff;

    ret = gp_widget_set_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set widget value to 1: %d\n", ret);
        goto out;
    }

    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set config tree to autofocus: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}


// Manual focusing a camera...
// xx is -3 / -2 / -1 / 0 / 1 / 2 / 3
// xx is -4 -3 / -2 / -1 / 0 / 1 / 2 - Canon EOS 750D
//
int DSLR::camera_manual_focus (Camera *camera, int xx, GPContext *context) {
    CameraWidget *widget = NULL, *child = NULL;
    CameraWidgetType type;
    int ret;
    float rval;
    char *mval;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, "manualfocusdrive", &child);
    if (ret < GP_OK)
    {
        fprintf (stderr, "lookup 'manualfocusdrive' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK)
    {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type)
    {
        case GP_WIDGET_RADIO:
        {
           int choices = gp_widget_count_choices (child);
            ret = gp_widget_get_value (child, &mval);
            if (ret < GP_OK)
            {
                fprintf (stderr, "could not get widget value: %d\n", ret);
                goto out;
            }
            if (choices == 7)
            { // see what Canon has in EOS_MFDrive
                ret = gp_widget_get_choice (child, xx+4, (const char**)&mval);
                if (ret < GP_OK)
                {
                    fprintf (stderr, "could not get widget radio choice %d: %d\n", xx+2, ret);
                    goto out;
                }
                fprintf(stderr,"GP_WIDGET_RADIO manual focus %d -> %s\n", xx, mval);
            }
            ret = gp_widget_set_value (child, mval);
            if (ret < GP_OK)
            {
                fprintf (stderr, "could not set widget value to 1: %d\n", ret);
                goto out;
            }
            break;
        }
        case GP_WIDGET_RANGE:
        ret = gp_widget_get_value (child, &rval);
        if (ret < GP_OK)
        {
            fprintf (stderr, "could not get widget value: %d\n", ret);
            goto out;
        }
        switch (xx) { // Range is on Nikon from -32768 <-> 32768
        case -3:	rval = -1024;break;
        case -2:	rval =  -512;break;
        case -1:	rval =  -128;break;
        case  0:	rval =     0;break;
        case  1:	rval =   128;break;
        case  2:	rval =   512;break;
        case  3:	rval =  1024;break;
        default:	rval = xx;	break; // hack
        }
        fprintf(stderr,"manual focus %d -> %f\n", xx, rval);
        ret = gp_widget_set_value (child, &rval);
        if (ret < GP_OK) {
            fprintf (stderr, "could not set widget value to 1: %d\n", ret);
            goto out;
        }
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }
    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK)
    {
        fprintf (stderr, "could not set config tree to autofocus: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}




static int _lookup_widget(CameraWidget*widget, const char *key, CameraWidget **child) {
    int ret;
    ret = gp_widget_get_child_by_name (widget, key, child);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widget, key, child);
    return ret;
}



// Gets a string configuration value.
//  This can be:
//   - A Text widget
//   - The current selection of a Radio Button choice
//   - The current selection of a Menu choice
//
//  Sample (for Canons eg):
//    get_config_value_string (camera, "owner", &ownerstr, context);
//
int DSLR::get_config_value_string (Camera *camera, const char *key, char **str, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;
    char			*val;

    ret = gp_camera_get_single_config (camera, key, &child, context);
    if (ret == GP_OK) {
        if (!child) fprintf(stderr,"child is NULL?\n");
        widget = child;
    } else {
        ret = gp_camera_get_config (camera, &widget, context);
        if (ret < GP_OK) {
            fprintf (stderr, "camera_get_config failed: %d\n", ret);
            return ret;
        }
        ret = _lookup_widget (widget, key, &child);
        if (ret < GP_OK) {
            fprintf (stderr, "lookup widget failed: %d\n", ret);
            goto out;
        }
    }

    // This type check is optional, if you know what type the label
    //  has already. If you are not sure, better check.
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    // This is the actual query call. Note that we just
    // a pointer reference to the string, not a copy...
    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not query widget value: %d\n", ret);
        goto out;
    }
    // Create a new copy for our caller.
    *str = strdup (val);
out:
    gp_widget_free (widget);
    return ret;
}


// Sets a string configuration value.
//  This can set for:
//   - A Text widget
//   - The current selection of a Radio Button choice
//   - The current selection of a Menu choice
//
//  Sample (for Canons eg):
//    get_config_value_string (camera, "owner", &ownerstr, context);
//
int DSLR::set_config_value_string (Camera *camera, const char *key, const char *val, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = _lookup_widget (widget, key, &child);
    if (ret < GP_OK) {
        fprintf (stderr, "lookup widget failed: %d\n", ret);
        goto out;
    }

    // This type check is optional, if you know what type the label
    //  has already. If you are not sure, better check.
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    // This is the actual set call. Note that we keep
    // ownership of the string and have to free it if necessary.

    ret = gp_widget_set_value (child, val);
    if (ret < GP_OK) {
        fprintf (stderr, "could not set widget value: %d\n", ret);
        goto out;
    }
    ret = gp_camera_set_single_config (camera, key, child, context);
    if (ret != GP_OK) {
        // This stores it on the camera again
        ret = gp_camera_set_config (camera, widget, context);
        if (ret < GP_OK) {
            fprintf (stderr, "camera_set_config failed: %d\n", ret);
            return ret;
        }
    }
out:
    gp_widget_free (widget);
    return ret;
}



//  This enables/disables the specific canon capture mode.
//
//  For non canons this is not required, and will just return
//  with an error (but without negative effects).

int DSLR::canon_enable_capture (Camera *camera, int onoff, GPContext *context) {
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;

    ret = gp_camera_get_single_config (camera, "capture", &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }

    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK) {
        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_TOGGLE:
        break;
    default:
        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }
    // Now set the toggle to the wanted value
    ret = gp_widget_set_value (child, &onoff);
    if (ret < GP_OK) {
        fprintf (stderr, "toggling Canon capture to %d failed with %d\n", onoff, ret);
        goto out;
    }
    // OK
    ret = gp_camera_set_single_config (camera, "capture", widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_set_config failed: %d\n", ret);
        return ret;
    }
out:
    gp_widget_free (widget);
    return ret;
}


/*
statis void DSLR::errordumper(GPLogLevel level, const char *domain, const char *str, void *data) {
  printf("%s\n", str);
}
*/
// This seems to have no effect on where images go
void set_capturetarget(Camera *canon, GPContext *canoncontext) {
    int retval;
    printf("Get root config.\n");
    CameraWidget *rootconfig; // okay, not really
    CameraWidget *actualrootconfig;
    retval = gp_camera_get_config(canon, &rootconfig, canoncontext);
    actualrootconfig = rootconfig;
    printf("  Retval: %d\n", retval);
    printf("Get main config.\n");
    CameraWidget *child;
    retval = gp_widget_get_child_by_name(rootconfig, "main", &child);
    printf("  Retval: %d\n", retval);
    printf("Get settings config.\n");
    rootconfig = child;
    retval = gp_widget_get_child_by_name(rootconfig, "settings", &child);
    printf("  Retval: %d\n", retval);
    printf("Get capturetarget.\n");
    rootconfig = child;
    retval = gp_widget_get_child_by_name(rootconfig, "capturetarget", &child);
    printf("  Retval: %d\n", retval);
    CameraWidget *capture = child;
    const char *widgetinfo;
    gp_widget_get_name(capture, &widgetinfo);
    printf("config name: %s\n", widgetinfo );
    const char *widgetlabel;
    gp_widget_get_label(capture, &widgetlabel);
    printf("config label: %s\n", widgetlabel);
    int widgetid;
    gp_widget_get_id(capture, &widgetid);
    printf("config id: %d\n", widgetid);
    CameraWidgetType widgettype;
    gp_widget_get_type(capture, &widgettype);
    printf("config type: %d == %d \n", widgettype, GP_WIDGET_RADIO);
    printf("Set value.\n");
    // capture to ram should be 0, although I think the filename also plays into
    // it
    int one=1;
    retval = gp_widget_set_value(capture, &one);
    printf("  Retval: %d\n", retval);
    printf("Enabling capture to CF.\n");
    retval = gp_camera_set_config(canon, actualrootconfig, canoncontext);
    printf("  Retval: %d\n", retval);
}


    void DSLR::capture_to_file(Camera *canon, GPContext *canoncontext, char *fn)
    {
    int fd, retval;
    CameraFile *canonfile;
    CameraFilePath camera_file_path;

    printf("Capturing.\n");

    retval = gp_camera_capture(canon, GP_CAPTURE_IMAGE, &camera_file_path, canoncontext);
    printf("  Retval: %d\n", retval);

    printf("Pathname on the camera: %s/%s\n", camera_file_path.folder, camera_file_path.name);

    if(fn)
        fd = open(fn, O_CREAT | O_WRONLY, 0644);
    else
        fd = open(camera_file_path.name, O_CREAT | O_WRONLY, 0644);
    retval = gp_file_new_from_fd(&canonfile, fd);
    printf("  Retval: %d\n", retval);
    retval = gp_camera_file_get(canon, camera_file_path.folder, camera_file_path.name,
             GP_FILE_TYPE_NORMAL, canonfile, canoncontext);
    printf("  Retval: %d\n", retval);

    printf("Deleting.\n");
    retval = gp_camera_file_delete(canon, camera_file_path.folder, camera_file_path.name,
            canoncontext);
    printf("  Retval: %d\n", retval);

    gp_file_free(canonfile);
}

void DSLR::on_pushButton_clicked()
{
    ui->labelMessages->setText(" Initialising camera... ");
    ui->labelMessages->adjustSize();
    Camera	*canon;
    int	i, retval;

// ????????????????????????????????????????????????
//	GPContext *canoncontext = sample_create_context();
    GPContext *canoncontext = gp_context_new();
    gp_camera_new(&canon);


//	gp_log_add_func(GP_LOG_ERROR, errordumper, NULL);

//     When I set GP_LOG_DEBUG instead of GP_LOG_ERROR above, I noticed that the
//     init function seems to traverse the entire filesystem on the camera.  This
//     is partly why it takes so long.
//     (Marcus: the ptp2 driver does this by default currently.)
//

    ui->labelMessages->setText(" Initialising camera... \nTakes about 10 seconds.");
    ui->labelMessages->adjustSize();
    retval = gp_camera_init(canon, canoncontext);
    if (retval != GP_OK)
    {
        QString rezMessage="Could not initialise camera.\nError code: ";
        rezMessage+=QString::number(retval);
        ui->labelMessages->setText(rezMessage);
        ui->labelMessages->adjustSize();
        return;
    }


// ??????????????????????????????????????????????????????????????????????????????
//	canon_enable_capture(canon, TRUE, canoncontext);
//    set_capturetarget(canon, canoncontext);


//    printf("Taking 100 previews and saving them to snapshot-XXX.jpg ...\n");
    int canonFocus=-3;
    for (i=0;i<10;i++)
    {
        CameraFile *file;
        char output_file[32];

        fprintf(stderr,"preview %d\n", i);
        retval = gp_file_new(&file);
        if (retval != GP_OK) {
            fprintf(stderr,"gp_file_new: %d\n", retval);
            return;
        }

//        autofocus every 10 shots
//		if (i%10 == 9) {
//			camera_auto_focus (canon, canoncontext);
//			camera_auto_focus (canon, canoncontext,0);
//		} else {
//			camera_manual_focus (canon, (i/10-5)/2, canoncontext);
//		}

// Canon: successively focus from -3 to 3
//		camera_manual_focus (canon, canonFocus, canoncontext);
        if(canonFocus>3)
            canonFocus=-3;
        else
            canonFocus++;


    set_config_value_string (canon, "eoszoom", "10", canoncontext);
 //   set_config_value_string (canon, "eoszoom", "5", canoncontext);
 //   set_config_value_string (canon, "eoszoom", "5", canoncontext);



#if 0 // testcase for EOS zooming
        {
//			char buf[20];
//			if (i<10) set_config_value_string (canon, "eoszoom", "5", canoncontext);
            if (i==2)
            {
                set_config_value_string (canon, "eoszoom", "5", canoncontext);
                set_config_value_string (canon, "eoszoom", "10", canoncontext);
            }
//			sprintf(buf,"%d,%d",(i&0x1f)*64,(i>>5)*64);
//			fprintf(stderr, "%d - %s\n", i, buf);
//			set_config_value_string (canon, "eoszoomposition", buf, canoncontext);
        }
#endif
        retval = gp_camera_capture_preview(canon, file, canoncontext);
        if (retval != GP_OK) {
            fprintf(stderr,"gp_camera_capture_preview(%d): %d\n", i, retval);
            return;
        }
        sprintf(output_file, "snapshot-%03d.jpg", i);
        retval = gp_file_save(file, output_file);
        if (retval != GP_OK) {
            fprintf(stderr,"gp_camera_capture_preview(%d): %d\n", i, retval);
            return;
        }
        gp_file_unref(file);

//        sprintf(output_file, "image-%03d.jpg", i);
//            capture_to_file(canon, canoncontext, output_file);

    }
    gp_camera_exit(canon, canoncontext);
//    return 0;

}

void DSLR::on_CapturePreviewButton_clicked()
{
    CaptureCameraPreview();
}

void DSLR::on_CaptureImage_clicked()
{
    if(!isCamera && !isCameraFile)
    {
        ui->labelMessages->setText("DSLR Camera not connected!");
        ui->labelMessages->adjustSize();
        return;
    }
//    capture_to_file(canon, canoncontext,"/run/shm/DSLR-SourceImage.jpg");
    set_config_value_string (canonCamera, "eosremoterelease", "Immediate", canonContext);
    sleep(3);
    set_config_value_string (canonCamera, "eosremoterelease", "Release Full", canonContext);
    camera_tether(canonCamera, canonContext,(char*)"/run/shm/srcDslr.cr2");
    NewCapture(fromCamera);
}

void DSLR::on_grabButton_clicked()
{
    CameraGrab();
}

void DSLR::on_releaseButton_clicked()
{
    CameraRelease();
}

void DSLR::on_targetButton_clicked()
{
    targetSelected=true;
//    cout << "Target X: " << targetPosition.x() << endl;
//    cout << "Target Y: " << targetPosition.y() << endl;
    dgeometry.CropAroundSelection(srcImage, &myImage, &processImage, targetPosition.x(), targetPosition.y(), true);
    GammaCorrection(myImage, (double)ui->horizontalGammaSlider->value()/1000, &myImage);
    NewCapture(fromCamera);
//    DisplayData();
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
    NewCapture(fromCamera);
}

void DSLR::on_focusMinus3Button_clicked()
{
    MoveCameraFocus(-2); // near 3
}

void DSLR::on_focusMinus2Button_clicked()
{
    set_config_value_string (canonCamera, "eoszoom", "1", canonContext);
    MoveCameraFocus(-3); // near 2
}

void DSLR::on_focusMinus1Button_clicked()
{
    set_config_value_string (canonCamera, "eoszoom", "5", canonContext);
    MoveCameraFocus(-4); // near 1
}

void DSLR::on_focusPlus1Button_clicked()
{
    set_config_value_string (canonCamera, "eoszoom", "10", canonContext);
    MoveCameraFocus(0); // Far 1
}

void DSLR::on_focusPlus2Button_clicked()
{
    MoveCameraFocus(1); // Far 2
}

void DSLR::on_focusPlus3Button_clicked()
{
    MoveCameraFocus(2); // Far 3
}


void DSLR::FrameMessage(QString message)
{
    message=" "+message+" ";
	ui->labelMessages->setText(message);
    ui->labelMessages->adjustSize();
//	#ifdef DEBUG
//	cout << message << endl;
    //	#endif
}

void DSLR::MoveCameraFocus(int value)
{
    if(!isCamera || !isCameraFile)
    {
        FrameMessage("DSLR Camera not connected!");
        return;
    }
    cameraFocus=value;
    camera_manual_focus (canonCamera, cameraFocus, canonContext);
//    CaptureCameraPreview();
    cameraFocus=-1; // none
    camera_manual_focus (canonCamera, cameraFocus, canonContext);
    CaptureCameraPreview();
}

void DSLR::CaptureCameraPreview()
{
#ifndef NODSLR
    if(!isCamera && !isCameraFile)
    {
        ui->labelMessages->setText("DSLR Camera not connected!");
        ui->labelMessages->adjustSize();
        return;
    }
    int retval;
    retval = gp_camera_capture_preview(canonCamera, canonFile, canonContext);
    if (retval != GP_OK)
    {
        dslrMessage="Could not capture camera preview.\nError code: ";
        dslrMessage+=QString::number(retval);
        ui->labelMessages->setText(dslrMessage);
        ui->labelMessages->adjustSize();
        return;
    }
    retval = gp_file_save(canonFile, "/run/shm/DSLR-SourceImage.jpg");
    if (retval != GP_OK)
    {
        dslrMessage="Could not save camera preview.\nError code: ";
        dslrMessage+=QString::number(retval);
        ui->labelMessages->setText(dslrMessage);
        ui->labelMessages->adjustSize();
        return;
    }
#else
    fromCamera=true;
#endif
    NewCapture(fromCamera);
}

void DSLR::on_resetButton_clicked()
{
    targetSelected=false;
    dgeometry.scaleX=0;
    dgeometry.scaleY=0;

//    dgeometry.sourceWidth=srcImage.cols;
//    dgeometry.sourceHeight=srcImage.rows;
//    dgeometry.absoluteWidth=srcImage.cols;
//    dgeometry.absoluteHeight=srcImage.rows;
//    dgeometry.scaleY=(double)dgeometry.relativeHeight/(double)dgeometry.absoluteHeight;
//    dgeometry.scaleX=dgeometry.scaleY;
//    dgeometry.scaleX=0.746875;
//    dgeometry.scaleY=0.746878;
//    dgeometry.offsetX=0;
//    dgeometry.offsetY=0;
//#ifdef IMAGELABELSVERTIACALALIGNMENTMIDDLE
//    if(dgeometry.absoluteWidth > dgeometry.absoluteHeight)
//        dgeometry.offsetY=(dgeometry.sourceHeight/dgeometry.sourceWidth*dgeometry.relativeWidth -\
//                           dgeometry.relativeHeight)/2/dgeometry.scaleY;
//#endif
//    dgeometry.sourceTargetX=dgeometry.absoluteWidth/2+dgeometry.offsetX;
//    dgeometry.sourceTargetY=dgeometry.absoluteHeight/2+dgeometry.offsetY;
//    dgeometry.absoluteTargetX=dgeometry.sourceTargetX-dgeometry.offsetX;
//    dgeometry.absoluteTargetY=dgeometry.sourceTargetY-dgeometry.offsetY;
//    dgeometry.relativeTargetX=dgeometry.absoluteTargetX*dgeometry.scaleX;
//    dgeometry.relativeTargetY=dgeometry.absoluteTargetY*dgeometry.scaleY;
    ui->horizontalGammaSlider->setValue(1000);
    ui->horizontalZoomSlider->setValue((int)dgeometry.scaleX);
//    dgeometry.DisplayGeometryData("After DSLR::reset");
    NewCapture(fromCamera);
    this->repaint();
}

void DSLR::on_connectButton_clicked()
{
    if(isCamera && isCameraFile)
    {
        CameraRelease();
        if(!isCamera || !isCameraFile)
        {
            QPixmap image("media/icons/tools-16x16/led-red.png");
            ui->resetLedLabel->setPixmap(image);
        }
    }
    else
    {
        CameraGrab();
        if(isCamera && isCameraFile)
        {
            QPixmap image("media/icons/tools-16x16/led-green.png");
            ui->resetLedLabel->setPixmap(image);
        }
    }

}
