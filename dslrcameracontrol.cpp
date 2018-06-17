#include "dslrcameracontrol.h"
#include "globalsettings.h"
//#include <QString>

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

using namespace std;

DslrCameraControl::DslrCameraControl()
{
    isCamera=false;
    isCameraFile=false;
    fromCamera=false;
    sourceFileName=new char(100);
    captureFileExtension = new char(8);
}

DslrCameraControl::~DslrCameraControl()
{
    delete sourceFileName;
    delete captureFileExtension;
}

void DslrCameraControl::CameraGrab()
{
    dslrMessage="";
    int	retval;
    if(!isCamera)
    {
        canonContext = gp_context_new();
        gp_camera_new(&canonCamera);
        retval = gp_camera_init(canonCamera, canonContext);
        if (retval != GP_OK)
        {
            dslrMessage+="Could not initialise camera.\nError code: ";
            dslrMessage+=QString::number(retval);
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
            dslrMessage="Could not create CameraFile object.\nError code: ";
            dslrMessage+=QString::number(retval);
            return;
        }
        isCameraFile=true;
        dslrMessage+="\nCamera file created.";
    }
    else
        dslrMessage+="\nCamera file already exists.";
    if(isCamera && isCameraFile)
        fromCamera=true;
    else
        fromCamera=false;
}

void DslrCameraControl::CameraRelease()
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
    if(isCamera && isCameraFile)
        fromCamera=true;
    else
        fromCamera=false;
}

int DslrCameraControl::SetCameraFocus(int value)
{
    if(!isCamera || !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    cameraFocus=value;
    if(CameraManualFocus(canonCamera, cameraFocus, canonContext) < GP_OK)
        return 0;
    cameraFocus=-1; // none
    if(CameraManualFocus(canonCamera, cameraFocus, canonContext) < GP_OK)
        return 0;
    CaptureCameraPreview();
    return 1;
}

int DslrCameraControl::CaptureCameraPreview()
{
#ifndef NODSLR
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    int retval;
    retval = gp_camera_capture_preview(canonCamera, canonFile, canonContext);
    if (retval != GP_OK)
    {
        dslrMessage="Could not capture camera preview.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    strcpy(sourceFileName,"/run/shm/DSLR-SourceImage.jpg");
//    retval = gp_file_save(canonFile, "/run/shm/DSLR-SourceImage.jpg");
    retval = gp_file_save(canonFile, sourceFileName);
    if (retval != GP_OK)
    {
        dslrMessage="Could not save camera preview.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
#else
    fromCamera=true;
#endif
    return 1;
}

int DslrCameraControl::SetEosZoomOrigin(double eosZoomPositionX, double eosZoomPositionY)
{
    if(!isCamera || !isCameraFile)
        return 0;
    if(eosZoomPositionX<0)
        eosZoomPositionX=0;
    if(eosZoomPositionY<0)
        eosZoomPositionY=0;
    QString eosZoomPositionString=QString::number((int)eosZoomPositionX)+","+QString::number((int)eosZoomPositionY);
    char bufferString[30];
    strcpy(bufferString,eosZoomPositionString.toLatin1());
    if(SetConfigValueString(canonCamera, "eoszoomposition", eosZoomPositionString.toLatin1(), canonContext) < GP_OK)
        return 0;
    return 1;
}

size_t DslrCameraControl::RunUnixCommandStoreOutput(char *param, char *result, int resultdim)
{
    //	runs an Unix command/program and returns the output into the result variable
        FILE *fp;
        int status;
        fp = popen(param, "r");
        size_t byte_count = fread(result, 1, resultdim-1, fp);
        if(byte_count>0)
            *(result+byte_count-1)=0;
        else
            *(result)=0;
        return byte_count;
}

void DslrCameraControl::KillGvfsdProcess()
{
    char buffer[50], command[50];
    int result=RunUnixCommandStoreOutput("ps -A | grep gvfsd-gphoto2", buffer, 50);
    if(result > 0)
    {
        int spacesTrimAtBeginning=strcspn(buffer,"0123456789");
        for(int j=spacesTrimAtBeginning; j<result; j++)
        {
            buffer[j-spacesTrimAtBeginning]=buffer[j];
            if(buffer[j]==32)
            {
                buffer[j-spacesTrimAtBeginning]=0;
                j=result;
            }
        }
        strcpy(command, "kill -9 ");
        strcat(command,buffer);
        RunUnixCommandStoreOutput(command, buffer, 50);
    }
}

int DslrCameraControl::SetMagnification(char* magnification)
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    if(SetConfigValueString(canonCamera, "eoszoom", magnification, canonContext) < GP_OK)
        return 0;
    return 1;
}



int DslrCameraControl::ShootOn()
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    if(SetConfigValueString(canonCamera, "eosremoterelease", "Immediate", canonContext) < GP_OK)
        return 0;
    return 1;
}

int DslrCameraControl::ShootRelease()
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    if(SetConfigValueString(canonCamera, "eosremoterelease", "Release Full", canonContext) < GP_OK)
        return 0;
    return 1;
}

void DslrCameraControl::GetCaptureFileExtension(char *inputString)
{
    int i;
    int dotPosition=strcspn(inputString,".");
    int counter=0;
    for(i=dotPosition; i<strlen(inputString); i++)
    {
        *(captureFileExtension+i-dotPosition)=*(inputString+i);
        counter++;
    }
    *(captureFileExtension+counter)=0;
}

int DslrCameraControl::ShootAndCapture(char *path, char *fileName, int exposureTime)
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    if(SetConfigValueString(canonCamera, "eosremoterelease", "Immediate", canonContext) < GP_OK)
        return 0;
    sleep(exposureTime);
    if(SetConfigValueString(canonCamera, "eosremoterelease", "Release Full", canonContext) < GP_OK)
        return 0;
    CameraTether(canonCamera, canonContext, path, fileName);
    GetCaptureFileExtension(fileName);
    return 1;
}

int DslrCameraControl::Connect()
{
    if(!isCamera || !isCameraFile)
    {
        KillGvfsdProcess();
        CameraGrab();
        if(isCamera && isCameraFile)
            return 1;
    }
    return 0;
}

int DslrCameraControl::Disconnect()
{
    if(isCamera && isCameraFile)
    {
        if(!SetMagnification("1"))
            return 0;
        CameraRelease();
        if(!isCamera && !isCameraFile)
            return 1;
    }
    return 0;
}

int DslrCameraControl::GetCameraListItem(CameraList *list, int selector, const char** foundItem, QString itemNameForErrorMessage)
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }

    int ret=gp_list_get_name(list, selector, foundItem);
    if( ret < GP_OK )
    {
        dslrMessage="Could not get " +itemNameForErrorMessage + " from the camera list.";
        return 0;
    }
    return 1;
}

int DslrCameraControl::GetCameraLastFile(Camera *camera, GPContext *context, const char *folder)
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    int ret;
    CameraList *list = 0;
    ret=gp_list_new(&list);
    ret=gp_filesystem_list_files(camera->fs, folder, list, context);
//    ret=gp_camera_folder_list_files(camera, folder, list, context);
    int listDim=gp_list_count(list);
    if(listDim)
    {
        const char* fileName=0;
        gp_list_get_name(list, listDim-1, &fileName);
        lastCameraFile=fileName;
    }
    else
    {
        lastCameraFile="";
        dslrMessage="The camera folder is empty!";
        lastFileNumber=0;
    }
    return 1;
    gp_list_free(list);
}

int DslrCameraControl::SetCameraLastFileNumber()
{
    if(!GetCameraFolder(canonCamera, canonContext,  "/", "ROOT", &rootCameraFolder))
        return 0;
    if(!GetCameraFolder(canonCamera, canonContext,  rootCameraFolder.toLatin1(), "DCIM", &dcimCameraFolder))
        return 0;
    if(!GetCameraFolder(canonCamera, canonContext,  dcimCameraFolder.toLatin1(), "LAST", &currentCameraFolder))
            return 0;
    if(!GetCameraLastFile(canonCamera, canonContext, currentCameraFolder.toLatin1()))
       return 0;
    if(lastCameraFile.length()==0)
    {
        lastFileNumber=0;
        return 1;
    }
    int lastPoint = lastCameraFile.lastIndexOf(".");
    QString lastFilenameNoExtension = lastCameraFile.left(lastPoint);
    lastFilenameNoExtension = lastFilenameNoExtension.right(4);
    lastFileNumber=lastFilenameNoExtension.toInt();
    return 1;
}

int DslrCameraControl::GetCameraFolder(Camera *camera, GPContext *context, const char *folderPrefix, QString selection, QString *cameraFolder)
{
    if(!isCamera && !isCameraFile)
    {
        dslrMessage="DSLR Camera not connected!";
        return 0;
    }
    int ret, selector;
    CameraList *list = 0;
    const char* folderName=0;
    ret=gp_list_new(&list);

//    ret=gp_camera_folder_list_folders(camera, folder, list, context);
    ret=gp_filesystem_list_folders(camera->fs, folderPrefix, list, context);
    if( ret < GP_OK )
    {
        dslrMessage="Could not get folder list.";
        {
            return 0;
            gp_list_free(list);
        }
    }
    int listDim=gp_list_count(list);
    if(listDim==0)
    {
        dslrMessage="Folder list is empty (no subfolders.";
        {
            return 0;
            gp_list_free(list);
        }
    }
    *cameraFolder = folderPrefix;
    if(selection=="ROOT")
    {
        if(!GetCameraListItem(list, 0, &folderName, "ROOT"))
        {
            return 0;
            gp_list_free(list);
        }
    }
    else
    {
        if(selection=="DCIM")
        {
            if(listDim>1)
            {
                bool folderFound=false;
                for(int i=0; i<listDim; i++)
                {
                    if(!GetCameraListItem(list, i, &folderName, "DCIM"))
                    {
                        return 0;
                        gp_list_free(list);
                    }
                    if( !strcmp(folderName, "DCIM") )
                    {
                        *cameraFolder += "/";
                        folderFound=true;
                        break;
                    }
                }
                if(!folderFound)
                {
                    dslrMessage="DCIM folder not found!";
                    gp_list_free(list);
                    return 0;
                }
            }
            else
            {
                if(!GetCameraListItem(list, 0, &folderName, "DCIM"))
                {
                    return 0;
                    gp_list_free(list);
                }
                if( !strcmp(folderName, "DCIM") )
                {
                    *cameraFolder += "/";
                }
                else
                {
                    dslrMessage="DCIM folder not found!";
                    gp_list_free(list);
                    return 0;
                }

            }
        }
        else
        {
            if(selection=="LAST")
            {
                if(listDim>0)
                {
                    bool folderFound=false;
                    QString folder0, folder1;
                    for(int i=0; i<listDim; i++)
                    {
                        if(!GetCameraListItem(list, i, &folderName, "LAST"))
                        {
                            return 0;
                            gp_list_free(list);
                        }
                        else
                            folderFound=true;
                        if(i==0)
                            folder0=folderName;
                        folder1=folderName;
                        if( folder1 > folder0 )
                        {
                            folder0=folder1;
                        }
                    }
                    if(!folderFound)
                    {
                        dslrMessage="Last camera folder not found!";
                        gp_list_free(list);
                        return 0;
                    }
                    else
                    {
                        *cameraFolder += "/";
                        *cameraFolder += folder0;
                        gp_list_free(list);
                        return 1;
                    }
                }
            }
        }
    }
    *cameraFolder += folderName;
    gp_list_free(list);
    return 1;
}



int DslrCameraControl::LookupWidget(CameraWidget *widget, const char *key, CameraWidget **child)
{
    int ret;
    ret = gp_widget_get_child_by_name (widget, key, child);
    if (ret < GP_OK)
        ret = gp_widget_get_child_by_label (widget, key, child);
    return ret;
}


int DslrCameraControl::SetCaptureTarget(Camera *canon, GPContext *canoncontext, char* option)
{ // For Canons, option will be "Memory card" for setting capture to SD card or
    // "Internal RAM" for setting capture to RAM. Keep capital/small letters as shown here!
    int retval;
    CameraWidget *rootconfig;
    CameraWidget *actualrootconfig;
    if( retval = gp_camera_get_config(canon, &rootconfig, canoncontext) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get camera configuration.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    actualrootconfig = rootconfig;
    CameraWidget *child;
    if( retval = gp_widget_get_child_by_name(rootconfig, "main", &child) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get main widget.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    rootconfig = child;
    if( retval = gp_widget_get_child_by_name(rootconfig, "settings", &child) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get settings widget.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    rootconfig = child;
    if(retval = gp_widget_get_child_by_name(rootconfig, "capturetarget", &child) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get capturetarget widget.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    CameraWidget *capture = child;

/*  ============================ These calls are unnecessary ===========================
    const char *widgetinfo;
    if(gp_widget_get_name(capture, &widgetinfo) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get widget name.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    const char *widgetlabel;
    if(gp_widget_get_label(capture, &widgetlabel) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get widget label.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    int widgetid;
    if(gp_widget_get_id(capture, &widgetid) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get widget ID.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    CameraWidgetType widgettype;
    if(gp_widget_get_type(capture, &widgettype) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not get widget type.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
  ======================================================================================
*/

    if(retval = gp_widget_set_value(capture, option) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not set widget value.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    if(retval = gp_camera_set_config(canon, actualrootconfig, canoncontext) < GP_OK)
    {
        dslrMessage+="SetCaptureTarget:\nCould not set camera configuration.\nError code: ";
        dslrMessage+=QString::number(retval);
        return 0;
    }
    return 1;
}




void DslrCameraControl::CameraTether(Camera *camera, GPContext *context,  char *destinationFolder, char *fileName)
{
    int fd, retval;
//    CameraFile *file;
    CameraEventType	evttype;
    CameraFilePath	*path;
    void	*evtdata;
    char saveFile[500];
//    strcpy (saveFile,"/run/shm/");
        strcpy (saveFile,(const char*)destinationFolder);
//        saveFile[strlen(destinationFolder)]=0;

    printf("Tethering...\n");

    while (1)
    {
        retval = gp_camera_wait_for_event (camera, 1000, &evttype, &evtdata, context);
        if (retval != GP_OK)
            break;
        switch (evttype)
        {

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
            strcpy(fileName, path->name);
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
int DslrCameraControl::camera_eosviewfinder(Camera *camera, GPContext *context, int onoff)
{
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret,val;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = LookupWidget (widget, "eosviewfinder", &child);
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

/*
int DslrCameraControl::camera_auto_focus(Camera *camera, GPContext *context, int onoff)
{
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret,val;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = LookupWidget (widget, "autofocusdrive", &child);
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
*/

// Manual focusing a camera...
// xx is -3 / -2 / -1 / 0 / 1 / 2 / 3
// xx is -4 -3 / -2 / -1 / 0 / 1 / 2 - Canon EOS 750D
//
int DslrCameraControl::CameraManualFocus (Camera *camera, int xx, GPContext *context)
{
    CameraWidget *widget = NULL, *child = NULL;
    CameraWidgetType type;
    int ret;
    float rval;
    char *mval;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK) {
        dslrMessage+="camera_get_config failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = LookupWidget (widget, "manualfocusdrive", &child);
    if (ret < GP_OK)
    {
        dslrMessage+="Lookup 'manualfocusdrive' failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "lookup 'manualfocusdrive' failed: %d\n", ret);
        goto out;
    }

    // check that this is a toggle
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK)
    {
        dslrMessage+="widget get type failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "widget get type failed: %d\n", ret);
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
                dslrMessage+="Could not get widget value.\nError code: ";
                dslrMessage+=QString::number(ret);
//                fprintf (stderr, "could not get widget value: %d\n", ret);
                goto out;
            }
            if (choices == 7)
            { // see what Canon has in EOS_MFDrive
                ret = gp_widget_get_choice (child, xx+4, (const char**)&mval);
                if (ret < GP_OK)
                {
                    dslrMessage+="Could not get widget radio choice ";
                    dslrMessage+=QString::number(xx+2) + ": ";
                    dslrMessage+=QString::number(ret);
//                    fprintf (stderr, "could not get widget radio choice %d: %d\n", xx+2, ret);
                    goto out;
                }
#ifdef DEBUG
                cout << "GP_WIDGET_RADIO manual focus " << xx << " -> " << mval << endl;
#endif
            }
            ret = gp_widget_set_value (child, mval);
            if (ret < GP_OK)
            {
                dslrMessage+="Could not set widget value to 1: ";
                dslrMessage+=QString::number(ret);
//                fprintf (stderr, "could not set widget value to 1: %d\n", ret);
                goto out;
            }
            break;
        }
        case GP_WIDGET_RANGE:
        ret = gp_widget_get_value (child, &rval);
        if (ret < GP_OK)
        {
            dslrMessage+="Could not get widget value.\nError code: ";
            dslrMessage+=QString::number(ret);
//            fprintf (stderr, "could not get widget value: %d\n", ret);
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
#ifdef DEBUG
        cout << "Manual focus " << xx << " -> " << rval << endl;
#endif
        ret = gp_widget_set_value (child, &rval);
        if (ret < GP_OK) {
            dslrMessage+="Could not set widget value to 1: ";
            dslrMessage+=QString::number(ret);
//            fprintf (stderr, "could not set widget value to 1: %d\n", ret);
            goto out;
        }
        break;
    default:
        dslrMessage+="Widget has bad type.\Type is: ";
        dslrMessage+=QString::number(type);
//        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }
    ret = gp_camera_set_config (camera, widget, context);
    if (ret < GP_OK)
    {
        dslrMessage+="Could not set config tree to autofocus.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "could not set config tree to autofocus: %d\n", ret);
        goto out;
    }
out:
    gp_widget_free (widget);
    return ret;
}


static int LookupWidget(CameraWidget*widget, const char *key, CameraWidget **child)
{
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
//    GetConfigValueString(camera, "owner", &ownerstr, context);
//
int DslrCameraControl::GetConfigValueString(Camera *camera, const char *key, char **str, GPContext *context)
{
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;
    char			*val;

    ret = gp_camera_get_single_config (camera, key, &child, context);
    if (ret == GP_OK) {
#ifdef DEBUG
        if (!child)
//            fprintf(stderr,"child is NULL?\n");
            cout << "child is NULL?" << endl;
#endif
        widget = child;
    } else {
        ret = gp_camera_get_config (camera, &widget, context);
        if (ret < GP_OK)
        {
            dslrMessage+="camera_get_config failed.\nError code: ";
            dslrMessage+=QString::number(ret);
//            fprintf (stderr, "camera_get_config failed: %d\n", ret);
            return ret;
        }
        ret = LookupWidget (widget, key, &child);
        if (ret < GP_OK)
        {
            dslrMessage+="Lookup widget failed.\nError code: ";
            dslrMessage+=QString::number(ret);
//            fprintf (stderr, "lookup widget failed: %d\n", ret);
            goto out;
        }
    }

    // This type check is optional, if you know what type the label
    //  has already. If you are not sure, better check.
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK)
    {
        dslrMessage+="Widget get type failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type)
    {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
        break;
    default:
        dslrMessage+="Widget has bad type.\nType is: ";
        dslrMessage+=QString::number(type);
//        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    // This is the actual query call. Note that we just
    // a pointer reference to the string, not a copy...
    ret = gp_widget_get_value (child, &val);
    if (ret < GP_OK)
    {
        dslrMessage+="Could not query widget value.\nError code: ";
        dslrMessage+=QString::number(ret);
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
//    SetConfigValueString(camera, "owner", &ownerstr, context);
//
int DslrCameraControl::SetConfigValueString (Camera *camera, const char *key, const char *val, GPContext *context)
{
    CameraWidget		*widget = NULL, *child = NULL;
    CameraWidgetType	type;
    int			ret;

    ret = gp_camera_get_config (camera, &widget, context);
    if (ret < GP_OK)
    {
        dslrMessage+="camera_get_config failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "camera_get_config failed: %d\n", ret);
        return ret;
    }
    ret = LookupWidget (widget, key, &child);
    if (ret < GP_OK)
    {
        dslrMessage+="Lookup widget failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "lookup widget failed: %d\n", ret);
        goto out;
    }

    // This type check is optional, if you know what type the label
    //  has already. If you are not sure, better check.
    ret = gp_widget_get_type (child, &type);
    if (ret < GP_OK)
    {
        dslrMessage+="Widget get type failed.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "widget get type failed: %d\n", ret);
        goto out;
    }
    switch (type) {
        case GP_WIDGET_MENU:
        case GP_WIDGET_RADIO:
        case GP_WIDGET_TEXT:
        break;
    default:
        dslrMessage+="Widget has bad type.\nType is: ";
        dslrMessage+=QString::number(type);
//        fprintf (stderr, "widget has bad type %d\n", type);
        ret = GP_ERROR_BAD_PARAMETERS;
        goto out;
    }

    // This is the actual set call. Note that we keep
    // ownership of the string and have to free it if necessary.

    ret = gp_widget_set_value (child, val);
    if (ret < GP_OK)
    {
        dslrMessage+="Could not set widget value.\nError code: ";
        dslrMessage+=QString::number(ret);
//        fprintf (stderr, "could not set widget value: %d\n", ret);
        goto out;
    }
    ret = gp_camera_set_single_config (camera, key, child, context);
    if (ret != GP_OK)
    {
        // This stores it on the camera again
        ret = gp_camera_set_config (camera, widget, context);
        if (ret < GP_OK)
        {
            dslrMessage+="camera_set_config failed.\nError code: ";
            dslrMessage+=QString::number(ret);
//            fprintf (stderr, "camera_set_config failed: %d\n", ret);
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
/*
int DslrCameraControl::canon_enable_capture (Camera *camera, int onoff, GPContext *context)
{
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
*/

/*
static void DslrCameraControl::errordumper(GPLogLevel level, const char *domain, const char *str, void *data)
{
    printf("%s\n", str);
}
*/
// This seems to have no effect on where images go



/*
void DslrCameraControl::capture_to_file(Camera *canon, GPContext *canoncontext, char *fn)
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
*/



